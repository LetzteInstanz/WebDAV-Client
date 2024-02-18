import QtQuick
import QtQuick.Controls as QtControls

QtControls.Button {
    id: button
    background: BorderRectangle {}
    contentItem: Text {
        text: parent.text
        font: parent.font
        color: enabled ? "#222222" : "#9f9f9f"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    onPressed: animation.start()

    SelectionSequentialAnimation {
        id: animation
        obj: button
    }
}
