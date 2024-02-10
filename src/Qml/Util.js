.pragma library

.import QtQml as QtQml

function createDlgAsync(comp, customFunc) {
    if (comp.status === QtQml.Component.Ready)
        customFunc(comp)
    else
        comp.statusChanged.connect(() => { customFunc(comp) })
}

function createDlg(comp, parent, typeName, properties) {
    if (comp.status === QtQml.Component.Error) {
        console.error("QML: " + typeName + ".qml component loading failed: ", comp.errorString());
        return;
    }
    const dlg = comp.createObject(parent, properties);
    if (dlg === null) {
        console.error("QML: " + typeName + " object creation failed");
        return;
    }
    console.debug("QML: " + typeName + " object was created")
    dlg.closed.connect(() => { console.debug("QML: " + typeName + " object was destroyed"); dlg.destroy() })
    return dlg
}
