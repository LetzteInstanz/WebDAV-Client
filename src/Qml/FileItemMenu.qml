import QtQuick.Controls

Menu {
    implicitWidth: 120 // todo: Find a solution to resize to the content.
    required property var viewModel
    required property var showSortDlgFunc
    required property var disconnectFunc

    MenuItem {
        text: qsTr("Check all to download")
        checkable: true
        checked: viewModel.areAllItemsCheckedToDownload()
        onTriggered: viewModel.checkAllToDownloadItems(!viewModel.areAllItemsCheckedToDownload())
    }
    MenuItem {
        text: qsTr("Download")
        enabled: viewModel.getCheckedToDownloadItemCount() > 0
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
