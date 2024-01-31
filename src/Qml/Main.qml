import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
//import Qt.labs.platform

import WebDavClient 1.0

ApplicationWindow {
    visible: true
    title: "WebDAVClient 1.0"
    background: BorderRectangle {}

    TextContextMenu { id: textContextMenu }
    EditServerDialog {
        id: addSrvDlg
        title: qsTr("Add server")
        onOpened: setData("", "", 80, "")
        onAccepted: srvListView.model.addServerInfo(desc(), addr(), port(), path())
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
            var model = srvListView.currentItem.model
            model.desc = desc(); model.addr = addr(); model.port = port(); model.path = path()
        }
    }
    // MessageDialog { // note: Doesn't work under Android in Qt6.6 version: https://forum.qt.io/topic/142589/messagedialog-not-working-on-android-qt6-4-0
    //     id: removeMsgDlg
    //     buttons: MessageDialog.Yes | MessageDialog.No
    //     onAccepted: srvListView.model.removeRow(srvListView.currentIndex)
    // }
    CustomMsgBox {
        id: removeMsgDlg
        standardButtons: Dialog.Yes | Dialog.No
        onAccepted: srvListView.model.removeRow(srvListView.currentIndex)
    }
    FolderDialog {
        id: pathDlg
        onAccepted: {
            var path = decodeURIComponent(pathDlg.selectedFolder)
            const prefix = "file://"
            if (path.startsWith(prefix))
                path = path.substring(prefix.length)

            pathTxtField.text = path
        }
    }
    Menu {
        id: editItemMenu
        implicitWidth: 100 // todo: Find a solution to resize to the content.

        MenuItem {
            text: qsTr("Edit")
            onTriggered: editSrvDlg.open()
        }
        MenuItem {
            text: qsTr("Remove")
            onTriggered: {
                removeMsgDlg.text = qsTr("Do you want to remove \"") + srvListView.currentItem.desc + qsTr("\"?")
                removeMsgDlg.open()
            }
        }
    }
    ProgressDialog {
        id: progressDlg
        onRejected: stackLayout.currentIndex = 0
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
                    text: qsTr("Log")
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
                        required property var model
                        required property string desc
                        required property string addr
                        required property int port
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
                                delayTimer.start()
                            }
                            onPressAndHold: (mouse) => {
                                var view = srvItemDelegate.ListView.view
                                view.currentIndex = index
                                editItemMenu.popup(view.currentItem, mouse.x, mouse.y)
                            }

                            Timer {
                                id: delayTimer
                                interval: 100
                                repeat: false;
                                onTriggered: {
                                    var item = srvListView.currentItem
                                    fileSystemModel.setServerInfo(item.addr, item.port, item.path)
                                    fileSystemModel.requestFileList()
                                    progressDlg.open()
                                    stackLayout.currentIndex = 1
                                }
                            }
                        }
                    }
                }
            }
        }

        TreeView {
            id: fileTreeView
            model: fileItemModel
            delegate: Item {
                id: fileItemDelegate
                implicitHeight: 96
                //width: TreeView.width
                //required property int index
                //required property var model
                required property string name
                required property string extension
                required property string datetime

                Row {
                    Image {
                        id: fileImage
                        //anchors.left: parent.left
                        width: 96
                        height: 96
                        source: "image://icons/" + extension
                    }
                    ColumnLayout {
                        Text {
                            id: filenameText
                            text: name;
                        }
                        Text {
                            id: fileTimeText
                            text: datetime;
                        }
                    }
                }
            }
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
                        settings.setDownloadPath(pathTxtField.text)
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
                    function hasChanges() { return settings.getDownloadPath() !== pathTxtField.text || settings.getCurrentLogLevel() !== logLevelComboBox.currentIndex }

                    Label {
                        text: qsTr("Path:")
                    }
                    Row {
                        width: parent.width
                        spacing: 5

                        TextField {
                            id: pathTxtField
                            width: parent.width - parent.spacing - pathButton.width
                            readOnly: true
                            onTextChanged: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
                            onReleased: (event) => { textContextMenu.hanldeReleaseEvent(pathTxtField, event) }
                        }
                        Button {
                            id: pathButton
                            text: qsTr("Select")
                            onClicked: {
                                pathDlg.currentFolder = encodeURIComponent("file://" + pathTxtField.text)
                                pathDlg.open()
                            }
                        }
                    }
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
