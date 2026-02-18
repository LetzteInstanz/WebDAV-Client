import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core

ApplicationWindow {
    visible: true
    title: "WebDAVClient 1.0"
    background: Core.BorderRectangle {}
    onClosing: (event) => {
        if (Qt.platform.os !== "android")
            return

        switch (stackLayout.currentIndex) {
            case 0:
                return

            case 1:
                stackLayout.currentIndex = 0
                stackLayout.children[1].destroy()
                break

            default:
                console.error(qsTr("QML: Unknown page index in the stack layout"))
                return
        }
        event.accepted = false
    }

    StackLayout {
        id: stackLayout
        anchors.fill: parent
        anchors.margins: 5
        currentIndex: 0

        ServerListPageColumnLayout {}
    }
}
