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
        console.error("QML: " + typeName + ".qml component loading failed: ", comp.errorString())
        return null
    }
    const obj = comp.createObject(parent, properties)
    if (obj === null) {
        console.error("QML: " + typeName + " object creation failed")
        return obj
    }
    console.debug("QML: " + typeName + " object was created")
    return obj
}

function createPopup(comp, parent, typeName, properties) {
    const popup = createObj(comp, parent, typeName, properties)
    if (popup === null)
        return popup

    popup.closed.connect(() => { console.debug("QML: " + typeName + " object was destroyed"); popup.destroy() })
    return popup
}

function showTextContextMenu(parent, textItem, event) {
    if (event.button !== Qt.RightButton)
        return

    if (!parent.textContextMenuComponent) {
        console.debug("QML: TextContextMenu.qml component isn't valid")
        parent.textContextMenuComponent = Qt.createComponent("TextContextMenu.qml", QtQml.Component.Asynchronous)
    }
    const comp = parent.textContextMenuComponent
    function createMenu() {
        const menu = createPopup(comp, parent, "TextContextMenu", {"textItem": textItem})
        menu.popup()
    }
    createObjAsync(comp, createMenu)
}
