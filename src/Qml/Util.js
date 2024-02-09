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
        console.error(typeName + ".qml component loading failed: ", comp.errorString());
        return;
    }
    const dlg = comp.createObject(parent, properties);
    if (dlg === null) {
        console.error(typeName + " object creation failed");
        return;
    }
    dlg.closed.connect(dlg.destroy)
    return dlg
}
