import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.labs.platform

import WebDavClient 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    title: "WebDAVClient 1.0"
    background: BorderRectangle {}
    property Component msgBoxComponent
    property Component editSrvDlgComponent
    property Component progressDlgComponent
    Component.onCompleted: {
        msgBoxComponent = Qt.createComponent("CustomMessageBox.qml", Component.Asynchronous);
        editSrvDlgComponent = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous);
        progressDlgComponent = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous);
    }

    TextContextMenu { id: textContextMenu }
    // FolderDialog {
    //     id: pathDlg
    //     onAccepted: {
    //         var path = decodeURIComponent(pathDlg.selectedFolder)
    //         const prefix = "file://"
    //         if (path.startsWith(prefix))
    //             path = path.substring(prefix.length)

    //         pathTxtField.text = path
    //     }
    // }
    StackLayout {
        id: stackLayout
        anchors.fill: parent
        anchors.margins: 5
        currentIndex: 0

        ServerListPageColumnLayout {
            settingsPage: settingsPageColumnLayout
        }

        FileListPageRectangle {}

        SettingsPageColumnLayout {
            id: settingsPageColumnLayout
        }

        LogPageColumnLayout {}
    }
}
