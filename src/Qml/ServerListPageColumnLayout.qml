import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util
import WebDavClient

ColumnLayout {
    property FileListPageColumnLayout fileListPage
    property SettingsPageColumnLayout settingsPage

    RowLayout {
        Button {
            text: qsTr("Add")
            onClicked: {
                function createDlg(comp) {
                    const dlg = Util.createPopup(comp, appWindow, "EditServerDialog", {"title": qsTr("Add server")})
                    if (dlg === null)
                        return

                    dlg.setData("", "", 80, "")
                    dlg.accepted.connect(() => { console.debug("QML: A new item was added to the server item model"); srvListView.model.addServerInfo(dlg.desc(), dlg.addr(), dlg.port(), dlg.path()) })
                    dlg.open()
                }

                Util.createObjAsync(editSrvDlgComponent, createDlg)
            }
        }
        Button {
            text: qsTr("Settings")
            onClicked: {
                settingsPage.prepare()
                stackLayout.currentIndex = 2
            }
        }
        Button {
            text: qsTr("Log")
            onClicked: stackLayout.currentIndex = 3
        }
    }
    Core.BorderRectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        ListView {
            id: srvListView
            anchors.fill: parent
            anchors.margins: 2
            spacing: 5
            model: itemModelManager.createModel(ItemModel.Server)
            clip: true
            currentIndex: -1
            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                width: ListView.view.width
                color: "lightgray"
            }
            property Component menuComponent
            Component.onCompleted: menuComponent = Qt.createComponent("ServerItemMenu.qml", Component.Asynchronous)
            delegate: Item {
                id: delegate
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
                        delegate.ListView.view.currentIndex = index
                        delayTimer.start()
                    }
                    onPressAndHold: (mouse) => {
                        const view = delegate.ListView.view
                        view.currentIndex = index

                        function createMenu(comp) {
                            const menu = Util.createPopup(comp, appWindow, "ServerItemMenu", {"view": srvListView})
                            menu.popup(view.currentItem, mouse.x, mouse.y)
                        }
                        Util.createObjAsync(srvListView.menuComponent, createMenu)
                    }

                    Timer {
                        id: delayTimer
                        interval: 100
                        repeat: false
                        onTriggered: {
                            function createDlg(comp) {
                                const dlg = Util.createPopup(comp, appWindow, "ProgressDialog", {})
                                if (dlg === null)
                                    return

                                const mainStackLayout = stackLayout
                                function requestFileList() {
                                    console.debug("QML: The first file list was requested")
                                    mainStackLayout.currentIndex = 1
                                    fileListPage.prepare()
                                    const item = srvListView.currentItem
                                    fileSystemModel.setServerInfo(item.addr, item.port)
                                    fileSystemModel.setRootPath(item.path)
                                    fileSystemModel.requestFileList("")
                                }
                                dlg.onOpened.connect(requestFileList)
                                const fsModel = fileSystemModel // note: An error occurs during closing the main window, if not to use the local variables
                                function disconnect() {
                                    mainStackLayout.currentIndex = 0
                                    console.debug("QML: The file system model is being disconnected")
                                    fsModel.disconnect()
                                }
                                dlg.rejected.connect(disconnect)
                                dlg.closed.connect(() => { mainStackLayout.enabled = true })
                                dlg.open()
                            }

                            stackLayout.enabled = false
                            Util.createObjAsync(progressDlgComponent, createDlg)
                        }
                    }
                }
            }
        }
    }
}
