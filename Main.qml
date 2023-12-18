import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import WebDavClient 1.0

ApplicationWindow {
    visible: true
    title: "WebDAVClient 1.0"

    EditServerDialog {
        id: addSrvDlg
        title: qsTr("Add server")
        onOpened: resetData()
        onAccepted: srvListView.model.add_server_info(desc(), addr(), port(), path())
    }
    EditServerDialog {
        id: editSrvDlg
        title: qsTr("Edit server")
        property var model
        onOpened: {
            setDesc(model.desc)
            setAddr(model.addr)
            setPort(model.port)
            setPath(model.path)
        }
        onAccepted: {
            model.desc = desc()
            model.addr = addr()
            model.port = port()
            model.path = path()
        }
    }
    MessageDialog {
        id: removeMsgDlg
        text: qsTr("Do you want to remove ") + '"' + desc + '"?'
        buttons: MessageDialog.Yes | MessageDialog.No
        property string desc
        onAccepted: srvListView.model.removeRow(srvListView.currentIndex)
    }
    StackLayout {
        id: stackLayout
        anchors.fill: parent
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
                    onClicked: removeMsgDlg.open()
                }
                Button {
                    id: settingsButton
                    text: qsTr("Settings")
                    onClicked: stackLayout.currentIndex = 2
                }
            }
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                ListView {
                    id: srvListView
                    anchors.fill: parent
                    model: srvItemModel
                    spacing: 5
                    bottomMargin: 2
                    leftMargin: 2
                    rightMargin: 2
                    topMargin: 2
                    highlightFollowsCurrentItem: true
                    currentIndex: -1

                    highlight: Rectangle {
                        width: ListView.view.width
                        color: "lightgray"
                    }
                    delegate: Rectangle {
                        id: delegate
                        color: "transparent"
                        width: ListView.view.width
                        height: descText.height + paramText.height
                        required property var model
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
                                delegate.ListView.view.currentIndex = index
                                editButton.enabled = index !== -1
                                removeButton.enabled = editButton.enabled
                                editSrvDlg.model = model
                                removeMsgDlg.desc = desc
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
                Layout.preferredHeight: backButton.height

                Button {
                    id: backButton
                    text: qsTr("Back")
                    onClicked: stackLayout.currentIndex = 0
                }
                Button {
                    id: okButton
                    anchors.right: parent.right
                    text: qsTr("Ok")
                    onClicked: stackLayout.currentIndex = 0 // todo: Save
                }
            }
            BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                TreeView {
                    anchors.fill: parent
                } // settings
            }
        }
    }
}
