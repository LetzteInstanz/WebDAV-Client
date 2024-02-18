import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../"
import "../Core" as Core
import WebDavClient

Dialog {
    anchors.centerIn: parent
    width: parent.width
    height: 370
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
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
    function enableOkButton() { standardButton(Dialog.Ok).enabled = listView.model.hasChanges() }
    function enableEditButtons() {
        const index = listView.currentIndex
        moveUpButton.enabled = index !== -1 && index !== 0
        moveDownButton.enabled = index !== -1 && index !== listView.count - 1
        invertButton.enabled = listView.count !== 0
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Higher priority")
            }
            Core.BorderRectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.verticalStretchFactor: 1

                Core.ListView {
                    id: listView
                    model: null
                    delegate: Item {
                        id: delegate
                        width: ListView.view.width
                        height: Math.max(paramNameText.contentHeight, descendingCheckBox.height)
                        required property var index
                        required property var model

                        RowLayout {
                            anchors.fill: parent

                            Text {
                                id: paramNameText
                                Layout.fillWidth: true
                                wrapMode: Text.Wrap
                                text: model.display

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        delegate.ListView.view.currentIndex = index
                                        enableEditButtons()
                                    }
                                }
                            }
                            CheckBox {
                                id: descendingCheckBox
                                text: qsTr("Descending")
                                checkState: model.descending ? Qt.Checked : Qt.Unchecked
                                onClicked: { model.descending = !model.descending; enableOkButton() }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: parent.clicked()
                                }
                            }
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
}
