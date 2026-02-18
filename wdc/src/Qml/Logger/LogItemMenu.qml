import QtQml
import QtQuick
import QtQuick.Controls

Menu {
    implicitWidth: 120 // todo: Find a solution to resize to the content.
    property ListView view

    MenuItem {
        text: qsTr("Copy")
        onTriggered: view.model.copyToClipboard(view.currentIndex)
    }
    MenuItem {
        text: qsTr("Copy all")
        onTriggered: view.model.copyAllToClipboard()
    }
}
