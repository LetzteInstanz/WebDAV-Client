import QtQuick

SequentialAnimation {
    property var obj

    PropertyAnimation {
        target: obj
        property: "scale"
        to: 1.2
        duration: 200
        easing.type: Easing.InOutQuad
    }
    PropertyAnimation {
        target: obj
        property: "scale"
        to: 1.0
        duration: 200
        easing.type: Easing.InOutQuad
    }
}
