import QtQml
import QtQuick
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util
import WebDavClient

Core.BorderRectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true
    function setViewModelAsync() {
        function setModel() {
            view.model = itemModelManager.createModel(ItemModel.File)
            view.model.modelReset.connect(() => { view.currentIndex = -1 })
            view.currentIndex = -1
            fileSystemModel.replyGot.disconnect(setModel)
        }
        fileSystemModel.replyGot.connect(setModel)
        fileSystemModel.errorOccurred.connect(() => { fileSystemModel.replyGot.disconnect(setModel) })
    }

    ListView {
        id: view
        anchors.fill: parent
        anchors.margins: 2
        spacing: 5
        model: null
        clip: true
        highlightFollowsCurrentItem: true
        highlight: Rectangle {
            width: ListView.view.width
            color: "lightgray"
        }
        property Component menuComponent
        property Component sortDlgComponent
        Component.onCompleted: {
            menuComponent = Qt.createComponent("FileItemMenu.qml", Component.Asynchronous)
            sortDlgComponent = Qt.createComponent("Sort/SortDialog.qml", Component.Asynchronous)
        }
        function destroyModel() {
            const model = view.model
            view.model = null
            model.destroy()
        }
        delegate: Core.BorderRectangle {
            id: delegate
            height: (Math.max(image.height, nameText.contentHeight + dateTimeText.contentHeight) + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin)
            width: ListView.view.width
            color: "transparent"
            required property int index
            required property var model

            RowLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.margins: 2

                Image {
                    id: image
                    source: "image://icons/" + model.iconName
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
                        text: model.name
                    }
                    Text {
                        id: dateTimeText
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.preferredHeight: 16
                        verticalAlignment: Text.AlignBottom
                        horizontalAlignment: Text.AlignRight
                        text: model.modificationTime
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    delegate.ListView.view.currentIndex = index
                    if (model.isFile)
                        return

                    delayTimer.start()
                }
                onPressAndHold: (mouse) => {
                    const attached = delegate.ListView.view
                    attached.currentIndex = index

                    function createMenu(comp) {
                        const menu = Util.createPopup(comp, view, "FileItemMenu", {"view": view})
                        menu.popup(attached.currentItem, mouse.x, mouse.y)
                    }
                    Util.createObjAsync(view.menuComponent, createMenu)
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

                            dlg.onOpened.connect(() => { console.debug("QML: A new file list was requested"); fileSystemModel.requestFileList(model.name) })
                            dlg.rejected.connect(() => { console.debug("QML: The request is being aborted"); fileSystemModel.abortRequest() })
                            dlg.open()
                        }

                        Util.createObjAsync(progressDlgComponent, createDlg)
                    }
                }
            }
        }
    }
}
