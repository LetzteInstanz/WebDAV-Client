import QtQuick.Controls

Menu {
    implicitWidth: 120 // todo: Find a solution to resize to the content.
    required property bool checkAllToDownloadItem
    required property bool enableDownloadItem
    required property var checkAllToDownloadItemsFunc
    required property var showSortDlgFunc
    required property var disconnectFunc

    MenuItem {
        text: qsTr("Check all to download")
        checkable: true
        checked: checkAllToDownloadItem
        onTriggered: checkAllToDownloadItemsFunc(!checkAllToDownloadItem)
    }
    MenuItem {
        text: qsTr("Download")
        enabled: enableDownloadItem
        //onTriggered: // todo: downLoad
    }
    MenuItem {
        text: qsTr("Sort")
        onTriggered: showSortDlgFunc()
    }
    MenuItem {
        text: qsTr("Disconnect")
        onTriggered: disconnectFunc()
    }
}
