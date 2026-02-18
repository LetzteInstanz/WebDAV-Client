import QtQuick
import QtQuick.Controls

import "Util.js" as Util

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    required property var openEditSrvDlgFunc
    required property var removeItemFunc

    MenuItem {
        text: qsTr("Edit")
        onTriggered: openEditSrvDlgFunc()
    }
    MenuItem {
        text: qsTr("Remove")
        onTriggered: removeItemFunc()
    }
}
