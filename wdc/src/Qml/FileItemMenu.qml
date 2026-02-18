import QtQml
import QtQuick.Controls

Menu {
    implicitWidth: 120 // todo: Find a solution to resize to the content.
    required property var sortFileItemModel
    required property var downloadFunc
    required property var showSortDlgFunc
    required property var disconnectFunc

    MenuItem {
        text: qsTr("Check all to download")
        checkable: true
        checked: sortFileItemModel.areAllItemsCheckedToDownload()
        onTriggered: sortFileItemModel.checkAllToDownloadItems(!sortFileItemModel.areAllItemsCheckedToDownload())
    }
    MenuItem {
        text: qsTr("Download")
        enabled: sortFileItemModel.sourceModel.getCheckedToDownloadItemCount() > 0
        onTriggered: downloadFunc()
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
