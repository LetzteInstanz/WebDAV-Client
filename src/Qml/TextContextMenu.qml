import QtQuick
import QtQuick.Controls

Menu {
    property Item textItem
    onOpened: {
        cutAction.enabled = !textItem.readOnly && textItem.selectedText.length > 0
        copyAction.enabled = textItem.selectedText.length > 0
        pasteAction.enabled = !textItem.readOnly
    }

    Action {
        id: cutAction
        text: qsTr("&Cut")
        icon.name: "edit-cut"
        shortcut: StandardKey.Cut
        onTriggered: textItem.cut()
    }
    Action {
        id: copyAction
        text: qsTr("&Copy")
        icon.name: "edit-copy"
        shortcut: StandardKey.Copy
        onTriggered: textItem.copy()
    }
    Action {
        id: pasteAction
        text: qsTr("&Paste")
        icon.name: "edit-paste"
        shortcut: StandardKey.Paste
        onTriggered: textItem.paste()
    }
    Action {
        id: selectAllAction
        text: qsTr("&Select all")
        icon.name: "edit-selectall"
        shortcut: StandardKey.SelectAll
        onTriggered: textItem.selectAll()
    }
}
