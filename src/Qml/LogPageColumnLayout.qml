import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Util.js" as Util

ColumnLayout {
    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: backButton.height

        Button {
            id: backButton
            text: qsTr("Back")
            onClicked: stackLayout.currentIndex = 0
        }
    }
    ScrollView {
        Layout.fillHeight: true
        Layout.fillWidth: true

        TextArea {
            id: logTxtArea
            background: BorderRectangle {}
            text: logger.getLog()
            readOnly: true
            textFormat: TextEdit.RichText
            onReleased: (event) => { Util.showTextContextMenu(appWindow, logTxtArea, event) }

            Connections {
                target: logger
                function onMsgReceived(msg) { logTxtArea.insert(logTxtArea.length, msg) }
            }
        }
    }
}
