.pragma library

.import QtQml as QtQml

function createObjAsync(comp, createObjFunc) {
    function wrapperFunc(status) {
        switch (status) {
            case QtQml.Component.Ready:
                createObjFunc(comp)
                break;

            case QtQml.Component.Error:
                console.error("QML: " + comp.url + ": " + qsTr("Component loading failed: "), comp.errorString())
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

function createObj(comp, parent, properties) {
    const obj = comp.createObject(parent, properties)
    if (obj === null) {
        console.error("QML: " + comp.url + ": " + qsTr("Object creation failed"))
        return null
    }
    console.debug("QML: " + comp.url + ": " + qsTr("Object was created"))
    obj.QtQml.Component.destruction.connect(() => { console.debug("QML: " + comp.url + ": " + qsTr("Object was destroyed")) })
    return obj
}

function createPopup(comp, parent, properties) {
    const popup = createObj(comp, parent, properties)
    if (popup === null)
        return null

    function destroy() {
        if (popup.parent) // note: If the popup is opened, an error occurs during closing the main window. This is a fix.
            popup.destroy()
    }
    popup.closed.connect(destroy)
    return popup
}

function createDialog(comp, parent, properties) { // note: this function is for the dialogs from QtQuick.Dialogs
    const dlg = createObj(comp, parent, properties)
    if (dlg === null)
        return null

    function destroy() { dlg.destroy() }
    dlg.accepted.connect(destroy)
    dlg.rejected.connect(destroy)
    return dlg
}
