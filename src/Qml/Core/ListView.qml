import QtQuick as QtQuick

QtQuick.ListView {
    anchors.fill: parent
    anchors.margins: 2
    spacing: 5
    clip: true
    highlightFollowsCurrentItem: true
    highlight: QtQuick.Rectangle {
        width: ListView.view.width
        color: "lightgray"
    }
}
