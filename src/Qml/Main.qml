import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import Qt.labs.platform

import "Core" as Core
import WebDavClient

ApplicationWindow {
    id: appWindow
    visible: true
    title: "WebDAVClient 1.0"
    background: Core.BorderRectangle {}
    onClosing: (event) => {
        if (Qt.platform.os !== "android")
            return

        switch (stackLayout.currentIndex) {
            case 0:
                return

            case 1:
                fileListPageColumnLayout.back()
                break

            case 2:
                settingsPageColumnLayout.back()
                break

            case 3:
                logPageColumnLayout.back()
                break

            default:
                console.error(qsTr("QML: Unknown page index in the stack layout"))
                return
        }
        event.accepted = false
    }
    property Component msgBoxComponent
    property Component editSrvDlgComponent
    property Component progressDlgComponent
    Component.onCompleted: {
        msgBoxComponent = Qt.createComponent("Core/MessageBox.qml", Component.Asynchronous)
        editSrvDlgComponent = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous)
        progressDlgComponent = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous)
    }
    property Component textContextMenuComponent

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
            fileListPage: fileListPageColumnLayout
            settingsPage: settingsPageColumnLayout
        }

        FileListPageColumnLayout {
            id: fileListPageColumnLayout
        }

        SettingsPageColumnLayout {
            id: settingsPageColumnLayout
        }

        LogPageColumnLayout {
            id: logPageColumnLayout
        }
    }
}
