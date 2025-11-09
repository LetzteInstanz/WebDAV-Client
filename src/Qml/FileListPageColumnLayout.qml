import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util

import WebDavClient

ColumnLayout {
    id: mainColumnLayout
    required property var backFunc
    required property string addr
    required property int port
    required property string path
    Component.onCompleted: {
        function createProgressDlg(comp) {
            const progressDlg = Util.createPopup(comp, ApplicationWindow.window, {"fileSystemModel": privateObj.fileSystemModel})
            function cancel() {
                backFunc()
                mainColumnLayout.destroy()
            }
            if (progressDlg === null) {
                cancel()
                return
            }
            progressDlg.rejected.connect(cancel)
            function setModel(fileSystemId) {
                console.assert(fileSystemId === 0, "File system ID must be 0")
                privateObj.fileSystemModel.ready.disconnect(setModel)
                listView.model = FileItemModelFactory.createModel(path)
                listView.currentIndex = -1
            }
            privateObj.fileSystemModel.ready.connect(setModel)
            privateObj.fileSystemModel.ready.connect(() => { currPathLabel.text = privateObj.fileSystemModel.getCurrentPath(0) })
            const fileSystemId = privateObj.fileSystemModel.requestFullData("", false)
            console.assert(fileSystemId === 0, "File system ID must be 0")
            progressDlg.open()
        }

        const comp = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous)
        Util.createObjAsync(comp, createProgressDlg)
    }
    Component.onDestruction: {
        if (listView.model)
            listView.model.destroy()

        privateObj.fileSystemModel.destroy()
    }

    QtObject {
        id: privateObj
        property ApplicationWindow mainWindow: mainColumnLayout.ApplicationWindow.window
        property var fileSystemModel: FileSystemModelFactory.createModel(addr, port)
        function showSortDlg() {
            function createDlg(comp) {
                const dlg = Util.createPopup(comp, mainWindow)
                if (dlg !== null)
                    dlg.open()
            }

            const comp = Qt.createComponent("Sort/SortDialog.qml", Component.Asynchronous)
            Util.createObjAsync(comp, createDlg)
        }
        function download() {
            console.debug(qsTr("QML: download() call"))
            const itemModel = listView.model.sourceModel

            function downloadWithConfirmation(downloadPath) {
                function createProgressDlg(comp) {
                    const fsModel = FileSystemModelFactory.createModel()
                    var info = fsInfoComp.createObject()
                    function count(fileSystemId) {
                        fsModel.count(fileSystemId, info)
                        fsModel.remove(fileSystemId)
                    }
                    const indexes = itemModel.getCheckedToDownloadIndexes()
                    const progressDlg = Util.createPopup(comp, mainWindow, {"indeterminate": false, "to": indexes.length, "fileSystemModel": fsModel, "countFunc": count})
                    if (progressDlg === null) {
                        fsModel.destroy()
                        info.destroy()
                        return
                    }
                    progressDlg.closed.connect(() => { fsModel.destroy() })
                    progressDlg.rejected.connect(() => { info.destroy() })
                    function showConfirmDlg() {
                        function createConfirmDlg(comp) {
                            const confirmDlg = Util.createPopup(comp, mainWindow, {"dirCount": info.directoryCount, "fileCount": info.fileCount, "sizeStr": info.getSizeStr(), "validAllSizes": info.areValidAllSizes, "invalidAllSizes": info.areInvalidAllSizes})
                            info.destroy()
                            if (confirmDlg === null)
                                return

                            confirmDlg.open()
                            // todo: begin to download
                        }
                        const comp = Qt.createComponent("ConfirmationDialog.qml", Component.Asynchronous)
                        Util.createObjAsync(comp, createConfirmDlg)
                    }
                    progressDlg.accepted.connect(showConfirmDlg)
                    progressDlg.open()
                    var isErrorCritical = false
                    fsModel.errorOccurred.connect((fileSystemId, text, isCritical) => { fsModel.abortAllRequests(); isErrorCritical = true })
                    var areOnlyFilesChecked = true
                    for (var i = 0; i < indexes.length; ++i) {
                        if (isErrorCritical)
                            return

                        const modelIndex = itemModel.index(indexes[i], 0)
                        const isFile = itemModel.data(modelIndex, Qml.FileItemModelRole.FileFlag)
                        console.assert(typeof isFile === "boolean")
                        if (isFile) {
                            const size = itemModel.data(modelIndex, Qml.FileItemModelRole.Size)
                            if (typeof size === "number") {
                                info.areInvalidAllSizes = false
                                info.size += size
                            } else {
                                info.areValidAllSizes = false
                            }
                            ++info.fileCount
                            ++progressDlg.value
                            continue
                        }
                        areOnlyFilesChecked = false
                        const path = itemModel.data(modelIndex, Qml.FileItemModelRole.Path)
                        console.assert(typeof path === "string")
                        ++info.directoryCount
                        fsModel.requestBasicData(path, true)
                    }
                    if (areOnlyFilesChecked)
                        progressDlg.accept()
                }

                const comp = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createProgressDlg)
            }

            if (!Settings.getAskPathFlag()) {
                downloadWithConfirmation(Settings.getDownloadPath())
                return
            }
            const folderDlg = Util.createDialog(folderDlgComp, mainWindow)
            if (folderDlg === null)
                return

            function acceptPath() {
                const path = Settings.removeScheme(folderDlg.selectedFolder)
                if (path === "") {
                    console.error(qsTr("QML: Invalid URI: " + folderDlg.selectedFolder))
                    return
                }
                console.debug(qsTr("QML: The download path is " + path))
                downloadWithConfirmation(path)
            }
            folderDlg.accepted.connect(acceptPath)
            folderDlg.open()
        }

        property Core.SelectionSequentialAnimation animation: Core.SelectionSequentialAnimation { obj: null }
        property Core.Timer delayTimer: Core.Timer {
            property var model: null
            onTriggered: {
                console.assert(!listView.enabled)
                function createProgressDlg(comp) {
                    const progressDlg = Util.createPopup(comp, privateObj.mainWindow, {"fileSystemModel": privateObj.fileSystemModel})
                    if (progressDlg === null) {
                        listView.enabled = true
                        return
                    }
                    function request() {
                        const fileSystemId = privateObj.fileSystemModel.requestFullData(model.name, false)
                        console.assert(fileSystemId === 0, "File system ID must be 0")
                    }
                    progressDlg.onOpened.connect(request)
                    progressDlg.closed.connect(() => { listView.enabled = true })
                    progressDlg.rejected.connect(() => { privateObj.fileSystemModel.abortRequest(0) })
                    progressDlg.open()
                }

                const comp = Qt.createComponent("ProgressDialog.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createProgressDlg)
            }
        }
        property Component folderDlgComp: Component{ FolderDialog {} }
        property Component fsInfoComp: Component { FileSystemInfo {} }
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
        id: listView // todo: support right-to-left writing
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: null
        delegate: Item { // todo: replace with Component
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
                    if (!model.isExit)
                        Util.createObjAsync(downloadComp, (comp) => { comp.createObject(contentRectangle, {}) })
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        const view = delegateItem.ListView.view
                        privateObj.animation.obj = view.itemAtIndex(index)
                        privateObj.animation.start()
                        if (model.isFile)
                            return

                        listView.enabled = false
                        privateObj.delayTimer.model = model
                        privateObj.delayTimer.start()
                    }
                    onPressAndHold: (event) => {
                        function createMenu(comp) {
                            const item = delegateItem.ListView.view.itemAtIndex(index)
                            const menu = Util.createPopup(comp, item, {"sortFileItemModel": listView.model, "showSortDlgFunc": privateObj.showSortDlg, "disconnectFunc": () => { backFunc(); mainColumnLayout.destroy() }, "downloadFunc": privateObj.download})
                            menu.popup(item, event.x, event.y)
                        }
                        const comp = Qt.createComponent("FileItemMenu.qml", Component.Asynchronous, listView)
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
                    id: downloadComp

                    CheckBox {
                        id: downloadCheckBox
                        x: parent.width - width
                        padding: 0
                        checkState: model.isReadyToDownload ? Qt.Checked : Qt.Unchecked
                        onClicked: model.isReadyToDownload = !model.isReadyToDownload
                    }
                }
            }
        }
    }
}
