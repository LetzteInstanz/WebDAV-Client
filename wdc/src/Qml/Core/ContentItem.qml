import QtQuick

Item {
    anchors.margins: 2
    anchors.topMargin: index === 0 ? 2 : 1
    anchors.bottomMargin: index === ListView.count - 1 ? 2 : 1
}
