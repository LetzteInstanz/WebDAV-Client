import QtQuick

ListView {
    anchors.fill: parent
    anchors.margins: 2
    spacing: 5
    clip: true
    highlightFollowsCurrentItem: true
    highlight: Rectangle {
        width: ListView.view.width
        color: "lightgray"
    }
}
