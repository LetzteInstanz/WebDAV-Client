import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util

ColumnLayout {
    function prepare() { logTxtArea.cursorPosition = logTxtArea.length - 1 }
    function back() { stackLayout.currentIndex = 0 }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: backButton.height

        Core.Button {
            id: backButton
            text: qsTr("Back")
            onClicked: back()
        }
    }
    ScrollView {
        Layout.fillHeight: true
        Layout.fillWidth: true
        background: Core.BorderRectangle {}
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }

        TextArea {
            id: logTxtArea
            anchors.fill: parent
            background: null
            text: logger.getLog()
            readOnly: true
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.RichText
            onReleased: (event) => { Util.showTextContextMenu(appWindow, logTxtArea, event) }

            Connections {
                target: logger
                function onMsgReceived(msg) { logTxtArea.insert(logTxtArea.length, msg) }
            }
        }
    }
}
