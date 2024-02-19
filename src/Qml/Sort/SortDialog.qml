import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../"
import "../Core" as Core
import WebDavClient

Dialog {
    anchors.centerIn: parent
    width: parent.width - 20
    height: 370 // todo: find a solution to resize to the content
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Sort parameters")
    onOpened: {
        listView.model = itemModelManager.createModel(ItemModel.SortParam)
        enableEditButtons()
        enableOkButton()
    }
    onClosed: {
        const model = listView.model
        listView.model = null
        model.destroy()
    }
    onAccepted: listView.model.save()
    background: Core.BorderRectangle {}
    contentItem: RowLayout {
        ColumnLayout {
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Higher priority")
            }
            Core.BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Core.ListView {
                    id: listView
                    anchors.fill: parent
                    model: null
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AlwaysOn
                    }
                    delegate: Item {
                        id: delegateItem
                        width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
                        height: Math.max(paramNameText.contentHeight, descendingCheckBox.height) + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin
                        required property int index
                        required property var model

                        Core.ContentItem {
                            id: contentItem
                            anchors.fill: parent

                            RowLayout {
                                anchors.fill: parent
                                spacing: 0

                                Text {
                                    id: paramNameText
                                    Layout.fillWidth: true
                                    wrapMode: Text.Wrap
                                    text: model.display
                                }
                                CheckBox {
                                    id: descendingCheckBox
                                    leftPadding: 0
                                    rightPadding: 0
                                    text: qsTr("Descending")
                                    checkState: model.descending ? Qt.Checked : Qt.Unchecked
                                    onClicked: { model.descending = !model.descending; enableOkButton() }
                                }
                            }
                        }
                        MouseArea {
                            id: paramNameTextMouseArea
                            width: contentItem.anchors.leftMargin + paramNameText.width
                            height: delegateItem.height
                            onClicked: {
                                const view = delegateItem.ListView.view
                                view.currentIndex = index
                                animation.obj = view.itemAtIndex(index)
                                animation.start()
                                enableEditButtons()
                            }
                        }
                        MouseArea {
                            x: paramNameTextMouseArea.width
                            width: delegateItem.width - paramNameTextMouseArea.width + contentItem.anchors.rightMargin
                            height: delegateItem.height
                            onClicked: descendingCheckBox.clicked()
                        }
                    }
                }
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Lower priority")
            }
        }
        Column {
            spacing: 5

            Core.Button {
                id: moveUpButton
                leftPadding: 0 // note: padding: 0 doesn't affect the paddings under Android
                rightPadding: 0
                width: 30
                text: "↑"
                onClicked: {
                    listView.model.moveUp(listView.currentIndex)
                    --listView.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
            Core.Button {
                id: invertButton
                leftPadding: 0 // note: padding: 0 doesn't affect the paddings under Android
                rightPadding: 0
                width: 30
                text: "⇅"
                onClicked: {
                    listView.model.invert()
                    listView.currentIndex = listView.count - 1 - listView.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
            Core.Button {
                id: moveDownButton
                leftPadding: 0 // note: padding: 0 doesn't affect the paddings under Android
                rightPadding: 0
                width: 30
                text: "↓"
                onClicked: {
                    listView.model.moveDown(listView.currentIndex)
                    ++listView.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
        }
    }
    function enableOkButton() { standardButton(Dialog.Ok).enabled = listView.model.hasChanges() }
    function enableEditButtons() {
        const index = listView.currentIndex
        moveUpButton.enabled = index !== -1 && index !== 0
        moveDownButton.enabled = index !== -1 && index !== listView.count - 1
        invertButton.enabled = listView.count !== 0
    }

    Core.SelectionSequentialAnimation {
        id: animation
        obj: null
    }
}
