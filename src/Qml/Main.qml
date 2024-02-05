import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
//import Qt.labs.platform

import WebDavClient 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    title: "WebDAVClient 1.0"
    background: BorderRectangle {}

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

        ServerListPageColumnLayout {}

        FileListPageRectangle {}

        ColumnLayout {
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: saveSettingsButton.height

                Button {
                    text: qsTr("Back")
                    onClicked: stackLayout.currentIndex = 0
                }
                Button {
                    id: saveSettingsButton
                    anchors.right: parent.right
                    text: qsTr("Ok")
                    onClicked: {
                        //settings.setDownloadPath(pathTxtField.text)
                        settings.setCurrentLogLevel(logLevelComboBox.currentIndex)
                        stackLayout.currentIndex = 0
                    }
                }
            }
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Column {
                    id: settingsColumnLayout
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing: 5
                    function hasChanges() { return /*settings.getDownloadPath() !== pathTxtField.text || */settings.getCurrentLogLevel() !== logLevelComboBox.currentIndex }

                    // Label {
                    //     text: qsTr("Path:")
                    // }
                    // Row {
                    //     width: parent.width
                    //     spacing: 5

                    //     TextField {
                    //         id: pathTxtField
                    //         width: parent.width - parent.spacing - pathButton.width
                    //         readOnly: true
                    //         onTextChanged: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
                    //         onReleased: (event) => { textContextMenu.hanldeReleaseEvent(pathTxtField, event) }
                    //     }
                    //     Button {
                    //         id: pathButton
                    //         text: qsTr("Select")
                    //         onClicked: {
                    //             pathDlg.currentFolder = encodeURIComponent("file://" + pathTxtField.text)
                    //             pathDlg.open()
                    //         }
                    //     }
                    // }
                    Label {
                        text: qsTr("Maximum log level:")
                    }
                    ComboBox {
                        id: logLevelComboBox
                        model: logLevelModel
                        delegate: ItemDelegate {
                            id: logLevelDelegate
                            text: modelData
                            required property string modelData
                        }
                        onActivated: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
                    }
                }
            }
        }

        ColumnLayout {
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: backButton2.height

                Button {
                    id: backButton2
                    text: qsTr("Back")
                    onClicked: stackLayout.currentIndex = 0
                }
            }
            ScrollView {
                Layout.fillHeight: true
                Layout.fillWidth: true

                TextArea {
                    id: logTxtArea
                    background: BorderRectangle {}
                    text: logger.getLog()
                    readOnly: true
                    onReleased: (event) => { textContextMenu.hanldeReleaseEvent(logTxtArea, event) }

                    Connections {
                        target: logger
                        function onMsgReceived(msg) { logTxtArea.insert(logTxtArea.length, msg + "\n") }
                    }
                }
            }
        }
    }
}
