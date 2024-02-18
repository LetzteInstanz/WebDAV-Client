.pragma library

.import QtQml as QtQml

function createObjAsync(comp, customFunc) {
    if (comp.status === QtQml.Component.Ready)
        customFunc(comp)
    else
        comp.statusChanged.connect(() => { customFunc(comp) })
}

function createObj(comp, parent, typeName, properties) {
    if (comp.status === QtQml.Component.Error) {
        console.error("QML: " + typeName + ".qml " + qsTr("component loading failed: "), comp.errorString())
        return null
    }
    const obj = comp.createObject(parent, properties)
    if (obj === null) {
        console.error("QML: " + typeName + qsTr(" object creation failed"))
        return obj
    }
    console.debug("QML: " + typeName + qsTr(" object was created"))
    return obj
}

function createPopup(comp, parent, typeName, properties) {
    const popup = createObj(comp, parent, typeName, properties)
    if (popup === null)
        return popup

    function destroy() {
        if (!popup.parent) // note: If the popup is opened, an error occurs during closing the main window. This is a fix.
            return

        console.debug("QML: " + typeName + qsTr(" object was destroyed"));
        popup.destroy()
    }
    popup.closed.connect(destroy)
    return popup
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
