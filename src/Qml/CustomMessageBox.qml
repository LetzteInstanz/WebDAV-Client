import QtQuick
import QtQuick.Controls

Dialog {
    id: dlg
    anchors.centerIn: parent
    modal: true
    property string text

    Label {
        id: txtLabel
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        text: dlg.text
    }
}
