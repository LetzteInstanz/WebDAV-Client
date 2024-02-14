import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../"
import "../Core" as Core
import WebDavClient

Dialog {
    anchors.centerIn: parent
    width: 300
    height: 250
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onOpened: {
        view.model = itemModelManager.createModel(ItemModel.SortParam)
        enableEditButtons()
        enableOkButton()
    }
    onClosed: {
        const model = view.model
        view.model = null
        model.destroy()
    }
    onAccepted: view.model.save()
    background: Core.BorderRectangle {}
    function enableOkButton() { standardButton(Dialog.Ok).enabled = view.model.hasChanges() }
    function enableEditButtons() {
        const index = view.currentIndex
        moveUpButton.enabled = index !== -1 && index !== 0
        moveDownButton.enabled = index !== -1 && index !== view.count - 1
        invertButton.enabled = view.count !== 0
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

                Core.CustomListView {
                    id: view
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

            Button {
                id: moveUpButton
                width: 30
                text: "↑"
                onClicked: {
                    view.model.moveUp(view.currentIndex)
                    --view.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
            Button {
                id: invertButton
                width: 30
                text: "↕"
                onClicked: {
                    view.model.invert()
                    view.currentIndex = view.count - 1 - view.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
            Button {
                id: moveDownButton
                width: 30
                text: "↓"
                onClicked: {
                    view.model.moveDown(view.currentIndex)
                    ++view.currentIndex
                    enableEditButtons()
                    enableOkButton()
                }
            }
        }
    }
}
