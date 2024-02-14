import QtQuick
import QtQuick.Controls

import "Util.js" as Util

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    property ListView view

    MenuItem {
        text: qsTr("Sort")
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createPopup(comp, appWindow, "SortDialog", {})
                if (dlg !== null)
                    dlg.open()
            }

            Util.createObjAsync(view.sortDlgComponent, createDlg)
        }
    }
    MenuItem {
        text: qsTr("Disconnect")
        onTriggered: {
            view.destroyModel()
            console.debug("QML: The file system model is being disconnected")
            fileSystemModel.disconnect()
            stackLayout.currentIndex = 0
        }
    }
}
