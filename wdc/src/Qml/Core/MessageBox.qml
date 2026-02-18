import QtQuick
import QtQuick.Controls

Dialog {
    id: dlg
    anchors.centerIn: parent
    modal: true
    closePolicy: Popup.CloseOnEscape
    background: BorderRectangle {}
    property string text

    contentItem: Label {
        id: txtLabel
        horizontalAlignment: Text.AlignHCenter
        text: dlg.text
        wrapMode: Text.Wrap
    }
}
