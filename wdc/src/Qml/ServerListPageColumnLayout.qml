import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util

ColumnLayout {
    Core.SelectionSequentialAnimation {
        id: animation
        obj: null
    }
    Core.Timer {
        id: delayTimer
        property Item item
        onTriggered: {
            console.assert(!listView.enabled)
            function createPage(comp) {
                const page = Util.createObj(comp, stackLayout, {"backFunc": () => { stackLayout.currentIndex = 0 }, "addr": item.addr, "port": item.port, "path": item.path})
                listView.enabled = true
                if (page !== null)
                    stackLayout.currentIndex = 1
            }

            const comp = Qt.createComponent("FileListPageColumnLayout.qml", Component.Asynchronous)
            Util.createObjAsync(comp, createPage)
        }
    }
    RowLayout {
        Core.Button {
            text: qsTr("Add")
            onClicked: {
                function createDlg(comp) {
                    const dlg = Util.createPopup(comp, ApplicationWindow.window, {"title": qsTr("Add server")})
                    if (dlg === null)
                        return

                    dlg.setData("", "", 80, "")
                    dlg.accepted.connect(() => { console.debug(qsTr("QML: A new item was added to the server item model")); listView.model.addServerInfo(dlg.desc(), dlg.addr(), dlg.port(), dlg.path()) })
                    dlg.open()
                }

                const comp = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createDlg)
            }
        }
        Core.Button {
            text: qsTr("Settings")
            onClicked: {
                function createPage(comp) {
                    const page = Util.createObj(comp, stackLayout, {"backFunc": () => { stackLayout.currentIndex = 0 }})
                    if (page !== null)
                        stackLayout.currentIndex = 1
                }
                const comp = Qt.createComponent("SettingsPageColumnLayout.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createPage)
            }
        }
        Core.Button {
            text: qsTr("Log")
            onClicked: {
                function createPage(comp) {
                    const page = Util.createObj(comp, stackLayout, {"backFunc": () => { stackLayout.currentIndex = 0 }})
                    if (page !== null)
                        stackLayout.currentIndex = 1
                }
                const comp = Qt.createComponent("Logger/LogPageColumnLayout.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createPage)
            }
        }
    }
    Core.ListView {
        id: listView
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: ServerItemModelFactory.createModel()
        currentIndex: -1
        Component.onDestruction: model.destroy()
        delegate: Item { // todo: replace with Component
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

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Text {
                        id: descText
                        Layout.fillWidth: true
                        font.bold: true
                        font.pointSize: 14
                        maximumLineCount: 1
                        wrapMode: Text.Wrap
                        elide: Text.ElideRight
                        text: desc
                    }
                    Text {
                        id: paramText
                        Layout.fillWidth: true
                        maximumLineCount: 1
                        wrapMode: Text.WrapAnywhere
                        elide: Text.ElideRight
                        text: "http://" + addr + ":" + port + path
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listView.enabled = false
                    const view = delegateItem.ListView.view
                    view.currentIndex = -1
                    const item = view.itemAtIndex(index)
                    animation.obj = item
                    animation.start()
                    delayTimer.item = item
                    delayTimer.start()
                }
                onPressAndHold: (event) => {
                    const view = delegateItem.ListView.view
                    view.currentIndex = index
                    const item = view.itemAtIndex(index)
                    animation.obj = item
                    animation.start()

                    function createMenu(comp) {
                        function openEditSrvDlg() {
                            const comp = Qt.createComponent("EditServerDialog.qml", Component.Asynchronous)
                            function createDlg(comp) {
                                const dlg = Util.createPopup(comp, ApplicationWindow.window, {"title": qsTr("Edit server")})
                                if (dlg === null)
                                    return

                                dlg.enableHasChangesFunc(true)
                                const item = view.currentItem
                                dlg.setData(item.desc, item.addr, item.port, item.path)
                                const model = item.model
                                function writeIntoModel() {
                                    console.debug(qsTr("QML: An item in the server item model was edited"))
                                    model.desc = dlg.desc(); model.addr = dlg.addr(); model.port = dlg.port(); model.path = dlg.path()
                                }
                                dlg.accepted.connect(writeIntoModel)
                                dlg.open()
                            }

                            Util.createObjAsync(comp, createDlg)
                        }

                        function removeServer() {
                            function createDlg(comp) {
                                const dlg = Util.createPopup(comp, ApplicationWindow.window, {"standardButtons": Dialog.Yes | Dialog.No, "title": qsTr("Confirmation"), "text": qsTr("Do you want to remove \"") + view.currentItem.desc + qsTr("\"?")})
                                if (dlg === null)
                                    return

                                const model = view.model
                                const index = view.currentIndex
                                dlg.accepted.connect(() => { console.debug(qsTr("QML: An item was removed from the server item model")); model.removeRow(index) })
                                dlg.open()
                            }

                            const comp = Qt.createComponent("Core/MessageBox.qml", Component.Asynchronous)
                            Util.createObjAsync(comp, createDlg)
                        }

                        const menu = Util.createPopup(comp, item, {"openEditSrvDlgFunc": openEditSrvDlg, "removeItemFunc": removeServer})
                        menu.popup(item, event.x, event.y)
                    }
                    const comp = Qt.createComponent("ServerItemMenu.qml", Component.Asynchronous, listView)
                    Util.createObjAsync(comp, createMenu)
                }
            }
        }
    }
}
