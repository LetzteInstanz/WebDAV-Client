import QtQuick
import QtQuick.Controls as QtControls

QtControls.Button {
    id: button
    background: BorderRectangle {
        id: borderRectangle
    }
    contentItem: Text {
        text: parent.text
        font: parent.font
        color: enabled ? "#222222" : "#9f9f9f"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    onPressed: animation.start()

    SequentialAnimation {
        id: animation

        PropertyAnimation {
            target: button
            property: "scale"
            to: 1.2
            duration: 200
            easing.type: Easing.InOutQuad
        }
        PropertyAnimation {
            target: button
            property: "scale"
            to: 1.0
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
}
