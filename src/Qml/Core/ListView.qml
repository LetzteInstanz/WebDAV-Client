import QtQuick as QtQuick

QtQuick.ListView {
    anchors.fill: parent
    leftMargin: 2
    rightMargin: 2
    topMargin: 2
    bottomMargin: 2
    spacing: 5
    clip: true
    highlightFollowsCurrentItem: true
    highlight: QtQuick.Rectangle {
        width: ListView.view.width
        color: "lightgray"
    }
}
