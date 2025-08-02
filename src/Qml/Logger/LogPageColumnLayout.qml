import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../Core" as Core
import "../Util.js" as Util

ColumnLayout {
    id: mainColumnLayout
    required property var backFunc
    Component.onCompleted: {
        listView.model = ItemModelManager.createModel(ItemModel.Log)
        listView.currentIndex = -1
        listView.positionViewAtEnd()
    }
    Component.onDestruction: listView.model.destroy()

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: backButton.height

        Core.Button {
            id: backButton
            text: qsTr("Back")
            onClicked: {
                backFunc()
                mainColumnLayout.destroy()
            }
        }
    }
    Core.ListView {
        Layout.fillHeight: true
        Layout.fillWidth: true
        id: listView
        model: null
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }
        delegate: Item {
            id: delegateItem
            width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
            height: messageText.contentHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin

            Core.ContentItem {
                id: contentItem
                anchors.fill: parent

                Text {
                    id: messageText
                    anchors.fill: parent
                    wrapMode: Text.Wrap
                    color: model.colour
                    text: model.text
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: delegateItem.ListView.view.currentIndex = index
                onPressAndHold: (event) => {
                    delegateItem.ListView.view.currentIndex = index
                    function createMenu(comp) {
                        const item = delegateItem.ListView.view.itemAtIndex(index)
                        const menu = Util.createPopup(comp, item, {"view": listView})
                        menu.popup(item, event.x, event.y)
                    }
                    const comp = Qt.createComponent("LogItemMenu.qml", Component.Asynchronous, listView)
                    Util.createObjAsync(comp, createMenu)
                }
            }
        }
    }
}
