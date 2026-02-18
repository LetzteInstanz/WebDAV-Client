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
        model: LogItemModelFactory.createModel()
        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AlwaysOn }
        delegate: Component {
            Core.ContentItem {
                width: ListView.view.width - ListView.view.leftMargin - ListView.view.rightMargin
                height: messageText.contentHeight + anchors.topMargin + anchors.bottomMargin

                RowLayout {
                    anchors.fill: parent

                    Text {
                        id: timeText
                        Layout.fillHeight: true
                        color: model.colour
                        text: model.time
                    }
                    Text {
                        id: messageText
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                        color: model.colour
                        text: model.text
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: parent.ListView.view.currentIndex = index
                    onPressAndHold: (event) => {
                        parent.ListView.view.currentIndex = index
                        function createMenu(comp) {
                            const item = parent.ListView.view.itemAtIndex(index)
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
}
