.pragma library

.import QtQml as QtQml

function createObjAsync(comp, createObjFunc) {
    function wrapperFunc(status) {
        switch (status) {
            case QtQml.Component.Ready:
                createObjFunc(comp)
                break;

            case QtQml.Component.Error:
                console.error("QML: " + qsTr("Component ") + comp.url + qsTr(" loading failed: "), comp.errorString())
                break;
        }
    }

    switch (comp.status) {
        case QtQml.Component.Ready:
        case QtQml.Component.Error:
            wrapperFunc(comp.status)
            break;

        default:
            comp.statusChanged.connect(wrapperFunc)
            break;
    }
}

function createObj(comp, parent, typeName, properties) {
    const obj = comp.createObject(parent, properties)
    if (obj === null) {
        console.error("QML: " + typeName + qsTr(" object creation failed")) // todo: use url property of Component type instead of typename
        return null
    }
    console.debug("QML: " + typeName + qsTr(" object was created"))
    return obj
}

function createPopup(comp, parent, typeName, properties) {
    const popup = createObj(comp, parent, typeName, properties)
    if (popup === null)
        return null

    function destroy() {
        if (!popup.parent) // note: If the popup is opened, an error occurs during closing the main window. This is a fix.
            return

        console.debug("QML: " + typeName + qsTr(" object was destroyed")); // todo: use url property of Component type instead of typename
        popup.destroy()
    }
    popup.closed.connect(destroy) // todo: use the attatched signals Component.onCompleted() and Component.onDestruction()
    return popup
}

function createDialog(comp, parent, typeName, properties) { // note: this function is for the dialogs from QtQuick.Dialogs
    const dlg = createObj(comp, parent, typeName, properties)
    if (dlg === null)
        return null

    function destroy() {
        console.debug("QML: " + typeName + qsTr(" object was destroyed")); // todo: use url property of Component type instead of typename
        dlg.destroy()
    }
    dlg.accepted.connect(destroy) // todo: use the attatched signals Component.onCompleted() and Component.onDestruction()
    dlg.rejected.connect(destroy)
    return dlg
}

function showTextContextMenu(parent, textItem, event) {
    if (event.button !== Qt.RightButton)
        return

    if (!parent.textContextMenuComponent) {
        console.debug(qsTr("QML: TextContextMenu.qml component isn't valid"))
        parent.textContextMenuComponent = Qt.createComponent("TextContextMenu.qml", QtQml.Component.Asynchronous)
    }
    const comp = parent.textContextMenuComponent
    function createMenu() {
        const menu = createPopup(comp, parent, "TextContextMenu", {"textItem": textItem})
        menu.popup()
    }
    createObjAsync(comp, createMenu)
}
