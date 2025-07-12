import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Logger" as Logger
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
        msgBoxComponent = Qt.createComponent("Core/MessageBox.qml", Component.Asynchronous, appWindow)
        editSrvDlgComponent = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous, appWindow)
        progressDlgComponent = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous, appWindow)
    }
    property Component textContextMenuComponent

    StackLayout {
        id: stackLayout
        anchors.fill: parent
        anchors.margins: 5
        currentIndex: 0

        ServerListPageColumnLayout {
            fileListPage: fileListPageColumnLayout
            settingsPage: settingsPageColumnLayout
            logPage: logPageColumnLayout
        }

        FileListPageColumnLayout {
            id: fileListPageColumnLayout
        }

        SettingsPageColumnLayout {
            id: settingsPageColumnLayout
        }

        Logger.LogPageColumnLayout {
            id: logPageColumnLayout
        }
    }
}
