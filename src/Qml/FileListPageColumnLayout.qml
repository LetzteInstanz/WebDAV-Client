import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util
import WebDavClient

ColumnLayout {
    function prepare() {
        currPathLabel.text = ""

        function setModel() {
            listView.model = itemModelManager.createModel(ItemModel.File)
            listView.model.modelReset.connect(() => { listView.currentIndex = -1 })
            listView.currentIndex = -1
            fileSystemModel.replyGot.disconnect(setModel)
        }
        fileSystemModel.replyGot.connect(setModel)
        fileSystemModel.errorOccurred.connect(() => { fileSystemModel.replyGot.disconnect(setModel) })
    }
    function back() {
        listView.destroyModel()
        console.debug(qsTr("QML: The file system model is being disconnected"))
        fileSystemModel.disconnect()
        stackLayout.currentIndex = 0
    }

    Connections {
        target: fileSystemModel
        function onReplyGot() { currPathLabel.text = fileSystemModel.getCurrentPath() }
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
            checkState: settings.getSearchCSFlag() ? Qt.Checked : Qt.Unchecked
            onClicked: {
                const cs = settings.getSearchCSFlag()
                settings.setSearchCSFlag(!cs)
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
    Core.BorderRectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        Core.ListView {
            id: listView
            model: null
            property Component menuComponent
            property Component sortDlgComponent
            Component.onCompleted: {
                menuComponent = Qt.createComponent("FileItemMenu.qml", Component.Asynchronous)
                sortDlgComponent = Qt.createComponent("Sort/SortDialog.qml", Component.Asynchronous)
            }
            function destroyModel() {
                const model = listView.model
                listView.model = null
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
                            const menu = Util.createPopup(comp, listView, "FileItemMenu", {"sortDlgComponent": listView.sortDlgComponent, "backFunc": back})
                            menu.popup(attached.currentItem, mouse.x, mouse.y)
                        }
                        Util.createObjAsync(listView.menuComponent, createMenu)
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
                                dlg.onOpened.connect(() => { console.debug(qsTr("QML: A new file list was requested")); fileSystemModel.requestFileList(model.name) })
                                dlg.rejected.connect(() => { console.debug(qsTr("QML: The request is being aborted")); fileSystemModel.abortRequest() })
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
