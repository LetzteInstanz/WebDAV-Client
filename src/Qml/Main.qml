import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import WebDavClient 1.0

ApplicationWindow {
    visible: true
    title: "WebDAVClient 1.0"
    background: BorderRectangle {}

    EditServerDialog {
        id: addSrvDlg
        title: qsTr("Add server")
        onOpened: setData("", "", 80, "")
        onAccepted: srvListView.model.add_server_info(desc(), addr(), port(), path())
    }
    EditServerDialog {
        id: editSrvDlg
        title: qsTr("Edit server")
        onOpened: {
            enableHasChangesFunc(true)
            var item = srvListView.currentItem
            setData(item.desc, item.addr, item.port, item.path)
        }
        onAccepted: {
            var item = srvListView.currentItem
            item.desc = desc(); item.addr = addr(); item.port = port(); item.path = path()
        }
    }
    MessageDialog {
        id: removeMsgDlg
        buttons: MessageDialog.Yes | MessageDialog.No
        onAccepted: srvListView.model.removeRow(srvListView.currentIndex)
    }
    FolderDialog {
        id: pathDlg
        onAccepted: {
            var path = pathDlg.selectedFolder.toString()
            const prefix = "file://"
            if (path.startsWith(prefix))
                path = path.substring(prefix.length)

            pathTxtField.text = path
        }
    }
    StackLayout {
        id: stackLayout
        anchors.fill: parent
        anchors.margins: 5
        currentIndex: 0

        ColumnLayout {
            RowLayout {
                Button {
                    id: addButton
                    text: qsTr("Add")
                    onClicked: addSrvDlg.open()
                }
                Button {
                    id: editButton
                    text: qsTr("Edit")
                    enabled: false
                    onClicked: editSrvDlg.open()
                }
                Button {
                    id: removeButton
                    text: qsTr("Delete")
                    enabled: false
                    onClicked: {
                        removeMsgDlg.text = qsTr("Do you want to remove ") + '"' + srvListView.currentItem.desc + '"?'
                        removeMsgDlg.open()
                    }
                }
                Button {
                    id: settingsButton
                    text: qsTr("Settings")
                    onClicked: {
                        pathTxtField.text = settings.getDownloadPath()
                        logLevelComboBox.currentIndex = settings.getCurrentLogLevel()
                        saveSettingsButton.enabled = false
                        stackLayout.currentIndex = 2
                    }
                }
                Button {
                    id: logButton
                    text: qsTr("Show log")
                    onClicked: stackLayout.currentIndex = 3
                }
            }
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                ListView {
                    id: srvListView
                    anchors.fill: parent
                    model: srvItemModel
                    clip: true
                    spacing: 5
                    anchors.margins: 2
                    highlightFollowsCurrentItem: true
                    currentIndex: -1

                    highlight: Rectangle {
                        width: ListView.view.width
                        color: "lightgray"
                    }
                    delegate: Rectangle {
                        id: srvItemDelegate
                        color: "transparent"
                        width: ListView.view.width
                        height: descText.height + paramText.height
                        required property int index
                        required property string desc
                        required property string addr
                        required property string port
                        required property string path

                        Text {
                            id: descText
                            font.bold: true
                            font.pointSize: 14
                            text: desc
                        }
                        Text {
                            id: paramText
                            anchors.top: descText.bottom
                            text: "http://" + addr + ":" + port + "/" + path
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                srvItemDelegate.ListView.view.currentIndex = index
                                editButton.enabled = index !== -1
                                removeButton.enabled = index !== -1
                            }
                        }
                    }
                }
            }
        }

        TreeView {
            id: fileTreeView
        }

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
                        var value = pathTxtField.text
                        if (settings.getDownloadPath() !== value)
                            settings.setDownloadPath(value)

                        value = logLevelComboBox.currentIndex
                        if (settings.getCurrentLogLevel() !== value)
                            settings.setCurrentLogLevel(value)

                        stackLayout.currentIndex = 0
                    }
                }
            }
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 5

                    ColumnLayout {
                        Layout.alignment: Qt.AlignTop

                        Label {
                            text: qsTr("Path:")
                            Layout.preferredHeight: pathTxtField.height
                            verticalAlignment: Text.AlignVCenter
                        }
                        Label {
                            text: qsTr("Maximum log level:")
                            Layout.preferredHeight: logLevelComboBox.height
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    ColumnLayout {
                        id: settingsFieldColumnLayout
                        Layout.alignment: Qt.AlignTop
                        function hasChanges() { return settings.getDownloadPath() !== pathTxtField.text || settings.getCurrentLogLevel() !== logLevelComboBox.currentIndex }

                        RowLayout {
                            TextField {
                                id: pathTxtField
                                Layout.fillWidth: true
                                readOnly: true
                                onTextChanged: saveSettingsButton.enabled = settingsFieldColumnLayout.hasChanges()
                            }
                            Button {
                                id: pathButton
                                text: qsTr("Select")
                                onClicked: {
                                    pathDlg.currentFolder = "file://" + pathTxtField.text
                                    pathDlg.open()
                                }
                            }
                        }
                        ComboBox {
                            id: logLevelComboBox
                            model: logLevelModel
                            delegate: ItemDelegate {
                                id: logLevelDelegate
                                text: modelData
                                required property string modelData
                            }
                            onActivated: saveSettingsButton.enabled = settingsFieldColumnLayout.hasChanges()
                        }
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
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                TextEdit {
                    id: logTxtEdit
                    anchors.fill: parent
                    text: logger.getLog()

                    Connections {
                        target: logger
                        function onMsgReceived(msg) { logTxtEdit.insert(logTxtEdit.length, msg) }
                    }
                }
            }
        }
    }
}
