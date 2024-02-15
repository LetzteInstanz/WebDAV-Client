import QtQuick
import QtQuick.Controls

import "Util.js" as Util

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    property Component sortDlgComponent
    property var backFunc

    MenuItem {
        text: qsTr("Sort")
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createPopup(comp, appWindow, "SortDialog", {})
                if (dlg !== null)
                    dlg.open()
            }

            Util.createObjAsync(sortDlgComponent, createDlg)
        }
    }
    MenuItem {
        text: qsTr("Disconnect")
        onTriggered: backFunc()
    }
}
