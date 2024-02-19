import QtQuick as QtQuick

QtQuick.ListView {
    leftMargin: 1
    rightMargin: 1
    topMargin: 1
    bottomMargin: 1
    clip: true
    highlightFollowsCurrentItem: true
    highlight: QtQuick.Rectangle {
        width: ListView.view.width
        color: "lightgray"
    }
}
