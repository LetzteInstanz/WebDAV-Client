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
    property LogPageColumnLayout logPage

    Core.SelectionSequentialAnimation {
        id: animation
        obj: null
    }
    Core.Timer {
        id: delayTimer
        property Item item
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createPopup(comp, appWindow, "ProgressDialog", {})
                if (dlg === null)
                    return

                const mainStackLayout = stackLayout
                function requestFileList() {
                    console.debug(qsTr("QML: The first file list was requested"))
                    mainStackLayout.currentIndex = 1
                    fileListPage.prepare()
                    const _item = item
                    fileSystemModel.setServerInfo(_item.addr, _item.port)
                    fileSystemModel.setRootPath(_item.path)
                    fileSystemModel.requestFileList("")
                }
                dlg.onOpened.connect(requestFileList)
                const fsModel = fileSystemModel // note: An error occurs during closing the main window, if not to use the local variables
                function disconnect() {
                    mainStackLayout.currentIndex = 0
                    console.debug(qsTr("QML: The file system model is being disconnected"))
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
    RowLayout {
        Core.Button {
            text: qsTr("Add")
            onClicked: {
                function createDlg(comp) {
                    const dlg = Util.createPopup(comp, appWindow, "EditServerDialog", {"title": qsTr("Add server")})
                    if (dlg === null)
                        return

                    dlg.setData("", "", 80, "")
                    dlg.accepted.connect(() => { console.debug(qsTr("QML: A new item was added to the server item model")); listView.model.addServerInfo(dlg.desc(), dlg.addr(), dlg.port(), dlg.path()) })
                    dlg.open()
                }

                Util.createObjAsync(editSrvDlgComponent, createDlg)
            }
        }
        Core.Button {
            text: qsTr("Settings")
            onClicked: {
                settingsPage.prepare()
                stackLayout.currentIndex = 2
            }
        }
        Core.Button {
            text: qsTr("Log")
            onClicked: {
                logPage.prepare()
                stackLayout.currentIndex = 3
            }
        }
    }
    Core.BorderRectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        Core.ListView {
            id: listView
            anchors.fill: parent
            model: itemModelManager.createModel(ItemModel.Server)
            currentIndex: -1
            property Component menuComponent
            Component.onCompleted: menuComponent = Qt.createComponent("ServerItemMenu.qml", Component.Asynchronous)
            delegate: Item {
                id: delegateItem
                width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
                height: descText.contentHeight + paramText.contentHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin
                required property int index
                required property var model
                required property string desc
                required property string addr
                required property int port
                required property string path

                Core.ContentItem {
                    id: contentItem
                    anchors.fill: parent

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
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        const view = delegateItem.ListView.view
                        view.currentIndex = -1
                        const item = view.itemAtIndex(index)
                        animation.obj = item
                        animation.start()
                        delayTimer.item = item
                        delayTimer.start()
                    }
                    onPressAndHold: (mouse) => {
                        const view = delegateItem.ListView.view
                        view.currentIndex = index
                        const item = view.itemAtIndex(index)
                        animation.obj = item
                        animation.start()

                        function createMenu(comp) {
                            const menu = Util.createPopup(comp, item, "ServerItemMenu", {"view": listView})
                            menu.popup(item, mouse.x, mouse.y)
                        }
                        Util.createObjAsync(listView.menuComponent, createMenu)
                    }
                }
            }
        }
    }
}
