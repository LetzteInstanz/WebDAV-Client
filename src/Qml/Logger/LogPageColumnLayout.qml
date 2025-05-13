import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../Core" as Core
import "../Util.js" as Util
import WebDavClient

ColumnLayout {
    function prepare() {
        listView.model = ItemModelManager.createModel(ItemModel.Log)
        listView.currentIndex = -1
        listView.positionViewAtEnd()
    }
    function back() {
        listView.destroyModel()
        stackLayout.currentIndex = 0
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: backButton.height

        Core.Button {
            id: backButton
            text: qsTr("Back")
            onClicked: back()
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
        property Component menuComponent
        Component.onCompleted: menuComponent = Qt.createComponent("LogItemMenu.qml", Component.Asynchronous)
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
                        const menu = Util.createPopup(comp, item, "LogItemMenu", {"view": listView})
                        menu.popup(item, event.x, event.y)
                    }
                    Util.createObjAsync(listView.menuComponent, createMenu)
                }
            }
        }
    }
}
