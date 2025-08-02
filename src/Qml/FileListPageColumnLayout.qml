import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util
import WebDavClient

ColumnLayout {
    function prepare() {
        searchTextField.text = ""
        currPathLabel.text = ""

        function setModel() {
            listView.model = ItemModelManager.createModel(ItemModel.File)
            listView.model.modelReset.connect(() => { listView.currentIndex = -1 })
            listView.currentIndex = -1
            FileSystemModel.replyGot.disconnect(setModel)
        }
        FileSystemModel.replyGot.connect(setModel)
        FileSystemModel.errorOccurred.connect(() => { FileSystemModel.replyGot.disconnect(setModel) })
    }
    function back() {
        listView.destroyModel()
        console.debug(qsTr("QML: The file system model is being disconnected"))
        FileSystemModel.disconnect()
        stackLayout.currentIndex = 0
    }

    Connections {
        target: FileSystemModel
        function onReplyGot() { currPathLabel.text = FileSystemModel.getCurrentPath() }
    }
    Core.SelectionSequentialAnimation {
        id: animation
        obj: null
    }
    Core.Timer {
        id: delayTimer
        property var model: null
        onTriggered: {
            function createDlg(comp) {
                const dlg = Util.createPopup(comp, appWindow, "ProgressDialog", {})
                if (dlg === null)
                    return

                const mainStackLayout = stackLayout
                dlg.onOpened.connect(() => { console.debug(qsTr("QML: A new file list was requested")); FileSystemModel.requestFileList(model.name) })
                dlg.rejected.connect(() => { console.debug(qsTr("QML: The request is being aborted")); FileSystemModel.abortRequest() })
                dlg.closed.connect(() => { mainStackLayout.enabled = true })
                dlg.open()
            }

            stackLayout.enabled = false
            Util.createObjAsync(progressDlgComponent, createDlg)
        }
    }
    RowLayout {
        TextField {
            id: searchTextField
            Layout.fillWidth: true
            placeholderText: qsTr("Search by name")
            onTextEdited: listView.model.searchWithTimer(text)

            Core.Button {
                anchors.right: parent.right
                height: parent.height
                width: height
                background: Item {}
                text: "×"
                onClicked: { searchTextField.clear(); listView.model.search(searchTextField.text) }
            }
        }
        CheckBox {
            text: qsTr("Case\nsensitive")
            checkState: Settings.getSearchCSFlag() ? Qt.Checked : Qt.Unchecked
            onClicked: {
                const cs = Settings.getSearchCSFlag()
                Settings.setSearchCSFlag(!cs)
                listView.model.repeatSearch(0)
            }
        }
    }
    Label {
        id: currPathLabel
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideMiddle
        font.bold: true
    }
    Core.ListView {
        id: listView
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: null
        delegate: Item {
            id: delegateItem
            height: Math.max(image.height, nameText.contentHeight + creationTimeText.contentHeight + Math.max(sizeText.contentHeight, modificationTimeText.contentHeight)) + contentRectangle.anchors.topMargin + contentRectangle.anchors.bottomMargin + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin
            width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
            required property int index
            required property var model

            Core.BorderRectangle {
                id: contentRectangle
                anchors.fill: parent
                anchors.margins: 2
                anchors.topMargin: index === 0 ? 2 : 1
                anchors.bottomMargin: index === listView.count - 1 ? 2 : 1
                color: "transparent"
                Component.onCompleted: {
                    if (model.isExit)
                        return

                    function createCheckBox(comp) { comp.createObject(contentRectangle, {}) }
                    Util.createObjAsync(downloadComponent, createCheckBox)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        const view = delegateItem.ListView.view
                        animation.obj = view.itemAtIndex(index)
                        animation.start()
                        if (model.isFile)
                            return

                        delayTimer.model = model
                        delayTimer.start()
                    }
                    onPressAndHold: (event) => {
                        function createMenu(comp) {
                            const item = delegateItem.ListView.view.itemAtIndex(index)
                            const check = listView.model.areAllItemsCheckedToDownload()
                            const enable = listView.model.getCheckedToDownloadItemCount() > 0
                            function checkAllItems(check) { listView.model.checkAllToDownloadItems(check) }
                            function showSortDlg() {
                                function createDlg(comp) {
                                    const dlg = Util.createPopup(comp, appWindow, "SortDialog", {})
                                    if (dlg !== null)
                                        dlg.open()
                                }

                                const comp = Qt.createComponent("Sort/SortDialog.qml", Component.Asynchronous)
                                Util.createObjAsync(comp, createDlg)
                            }
                            const menu = Util.createPopup(comp, item, "FileItemMenu", {"checkAllToDownloadItem": check, "enableDownloadItem": enable, "checkAllToDownloadItemsFunc": checkAllItems, "showSortDlgFunc": showSortDlg, "disconnectFunc": back})
                            menu.popup(item, event.x, event.y)
                        }
                        const comp = Qt.createComponent("FileItemMenu.qml", Component.Asynchronous)
                        Util.createObjAsync(comp, createMenu)
                    }
                }
                RowLayout {
                    id: rowLayout
                    anchors.fill: parent
                    anchors.rightMargin: 2
                    anchors.bottomMargin: 2
                    spacing: 0

                    Image {
                        id: image
                        Layout.preferredWidth: model.needsWideImageWidth ? 54 : 48 // note: Some icons have more narrow transparent "border";
                        fillMode: Image.Pad                                        // There are different spacing sizes between nameText item and the image because of that
                        smooth: false
                        source: "image://icons/" + model.iconName
                    }
                    ColumnLayout {
                        spacing: 0

                        Text {
                            id: nameText
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.bold: true
                            font.pointSize: 14
                            wrapMode: Text.Wrap
                            text: model.name
                        }
                        Text {
                            id: creationTimeText
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                            text: model.creationTime
                        }
                        RowLayout {
                            spacing: 0

                            Text {
                                id: sizeText
                                Layout.fillWidth: true
                                verticalAlignment: Text.AlignBottom
                                text: model.size
                            }
                            Text {
                                id: modificationTimeText
                                verticalAlignment: Text.AlignBottom
                                horizontalAlignment: Text.AlignRight
                                text: model.modificationTime
                            }
                        }
                    }
                }
                Component {
                    id: downloadComponent

                    CheckBox {
                        id: downloadCheckBox
                        x: parent.width - width
                        padding: 0
                        property var row
                        Component.onCompleted: { row = index; console.debug(qsTr("Download check box is created") + " (row = " + row + ")") }
                        Component.onDestruction: console.debug(qsTr("Download check box is destroyed") + " (row = " + row + ")")
                        checkState: model.isReadyToDownload ? Qt.Checked : Qt.Unchecked
                        onClicked: model.isReadyToDownload = !model.isReadyToDownload
                    }
                }
            }
        }
    }
}
