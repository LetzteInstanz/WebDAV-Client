import QtQml
import QtQuick
import QtQuick.Controls

import "Util.js" as Util

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    property ListView view

    MenuItem {
        text: qsTr("Edit")
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createDlg(comp, appWindow, "EditServerDialog", {"title": qsTr("Edit server")})
                if (dlg === null)
                    return;

                dlg.enableHasChangesFunc(true)
                const item = view.currentItem
                dlg.setData(item.desc, item.addr, item.port, item.path)
                function writeIntoModel() {
                    console.debug("QML: An item in the server view model was edited")
                    const model = view.currentItem.model
                    model.desc = dlg.desc(); model.addr = dlg.addr(); model.port = dlg.port(); model.path = dlg.path()
                }
                dlg.accepted.connect(writeIntoModel)
                dlg.open()
            }

            Util.createDlgAsync(editSrvDlgComponent, createDlg)
        }
    }
    MenuItem {
        text: qsTr("Remove")
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createDlg(comp, appWindow, "CustomMessageBox", {"standardButtons": Dialog.Yes | Dialog.No, "text": qsTr("Do you want to remove \"") + view.currentItem.desc + qsTr("\"?")})
                if (dlg === null)
                    return

                dlg.accepted.connect(() => { console.debug("QML: An item was removed from the server view model"); view.model.removeRow(view.currentIndex) })
                dlg.open()
            }

            Util.createDlgAsync(msgBoxComponent, createDlg)
        }
    }
}
