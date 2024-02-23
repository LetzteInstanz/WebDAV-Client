import QtQuick as QtQuick

QtQuick.ListView {
    leftMargin: 1
    rightMargin: 1
    topMargin: 1
    bottomMargin: 1
    clip: true
    highlightFollowsCurrentItem: true
    highlightMoveDuration: 600
    highlightResizeDuration: 1000
    highlight: QtQuick.Rectangle {
        width: ListView.view.width
        color: "lightgray"
    }
    BorderRectangle {
        anchors.fill: parent
        color: "transparent"
    }
    function destroyModel() {
        const model_ = model
        model = null
        model_.destroy()
    }
}
