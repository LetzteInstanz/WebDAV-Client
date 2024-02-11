import QtQuick
import QtQuick.Controls

Menu {
    implicitWidth: 100 // todo: Find a solution to resize to the content.
    property ListView view

    MenuItem {
        text: qsTr("Disconnect")
        onTriggered: {
            console.debug("QML: The file view model property is null now")
            view.model = null
            console.debug("QML: The file system model is being disconnected")
            fileSystemModel.disconnect()
            stackLayout.currentIndex = 0
        }
    }
}
