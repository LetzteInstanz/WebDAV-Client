import QtQuick
import QtQuick.Controls

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    property ListView view

    MenuItem {
        text: qsTr("Edit")
        property Component editSrvDlgComponent
        Component.onCompleted: editSrvDlgComponent = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous);
        onTriggered: {
            function createDlg() {
                const comp = editSrvDlgComponent
                if (comp.status === Component.Error) {
                    console.error("EditServerDialog.qml component loading failed: ", comp.errorString());
                    return;
                }
                const dlg = comp.createObject(appWindow, {"title": qsTr("Edit server")});
                if (dlg === null) {
                    console.error("EditServerDialog object creation failed");
                    return;
                }
                dlg.enableHasChangesFunc(true)
                const item = view.currentItem
                dlg.setData(item.desc, item.addr, item.port, item.path)
                function writeIntoModel() {
                    const model = view.currentItem.model
                    model.desc = dlg.desc(); model.addr = dlg.addr(); model.port = dlg.port(); model.path = dlg.path()
                }
                dlg.accepted.connect(writeIntoModel)
                dlg.closed.connect(dlg.destroy)
                dlg.open()
            }

            const comp = editSrvDlgComponent
            if (comp.status === Component.Ready)
                createDlg();
            else
                comp.statusChanged.connect(createDlg);
        }
    }
    MenuItem {
        text: qsTr("Remove")
        property Component msgBoxComponent
        Component.onCompleted: msgBoxComponent = Qt.createComponent("CustomMessageBox.qml", Component.Asynchronous);
        onTriggered: {
            function createDlg() {
                const comp = msgBoxComponent
                if (comp.status === Component.Error) {
                    console.error("CustomMessageBox.qml component loading failed: ", comp.errorString());
                    return;
                }
                const dlg = comp.createObject(appWindow, {"standardButtons": Dialog.Yes | Dialog.No, "text": qsTr("Do you want to remove \"") + view.currentItem.desc + qsTr("\"?")});
                if (dlg === null) {
                    console.error("CustomMessageBox object creation failed");
                    return;
                }
                dlg.accepted.connect(() => { view.model.removeRow(view.currentIndex) })
                dlg.closed.connect(dlg.destroy)
                dlg.open()
            }

            const comp = msgBoxComponent
            if (comp.status === Component.Ready)
                createDlg();
            else
                comp.statusChanged.connect(createDlg);
        }
    }
}
