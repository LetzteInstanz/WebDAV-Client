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
            property Component msgBoxComponent
            Component.onCompleted: msgBoxComponent = Qt.createComponent("CustomMessageBox.qml", Component.Asynchronous);
            onTriggered: {
                function createDlg() {
                    const comp = msgBoxComponent
                    if (comp.status === Component.Error) {
                        console.error("CustomMessageBox.qml component loading failed: ", comp.errorString());
                        return;
                    }
                    const dlg = comp.createObject(appWindow, {"standardButtons": Dialog.Yes | Dialog.No, "text": qsTr("Do you want to remove \"") + srvListView.currentItem.desc + qsTr("\"?")});
                    if (dlg === null) {
                        console.error("CustomMessageBox object creation failed");
                        return;
                    }
                    dlg.accepted.connect(() => { srvListView.model.removeRow(srvListView.currentIndex) })
                    dlg.closed.connect(dlg.destroy)
                    dlg.open()
                }

                const comp = msgBoxComponent
                if (comp.status === Component.Ready)
                    createDlg();
                else
                    comp.statusChanged.connect(createDlg);
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
                    anchors.margins: 2
                    spacing: 5
                    highlightFollowsCurrentItem: true
                    currentIndex: -1

                    highlight: Rectangle {
                        width: ListView.view.width
                        color: "lightgray"
                    }
                    delegate: Item {
                        id: srvItemDelegate
                        width: ListView.view.width
                        height: descText.contentHeight + paramText.contentHeight
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
                            elide: Text.ElideRight
                            text: desc
                        }
                        Text {
                            id: paramText
                            anchors.top: descText.bottom
                            elide: Text.ElideRight
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

        BorderRectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                anchors.fill: parent
                model: fileItemModel
                clip: true
                anchors.margins: 2
                spacing: 5

                delegate: BorderRectangle {
                    height: (Math.max(fileImage.height, nameText.contentHeight + dateTimeText.contentHeight) + fileItemDelegateRowLayout.anchors.topMargin + fileItemDelegateRowLayout.anchors.bottomMargin)
                    width: ListView.view.width
                    required property string name
                    required property string extension
                    required property string datetime

                    RowLayout {
                        id: fileItemDelegateRowLayout
                        anchors.fill: parent
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2
                        anchors.topMargin: 2

                        Image {
                            id: fileImage
                            source: "image://icons/" + extension
                        }
                        ColumnLayout {
                            Text {
                                id: nameText
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                Layout.verticalStretchFactor: 1
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignLeft
                                font.bold: true
                                font.pointSize: 14
                                wrapMode: Text.Wrap
                                text: name;
                            }
                            Text {
                                id: dateTimeText
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                Layout.preferredHeight: 16
                                verticalAlignment: Text.AlignBottom
                                horizontalAlignment: Text.AlignRight
                                text: datetime;
                            }
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
