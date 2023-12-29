import QtQml
import QtQuick
import QtQuick.Controls

Menu {
    function hanldeReleaseEvent(textItem, event) {
        if (event.button !== Qt.RightButton)
            return

        curData._textItem = textItem
        cutAction.enabled = !curData._textItem.readOnly && curData._textItem.selectedText.length > 0
        copyAction.enabled = curData._textItem.selectedText.length > 0
        pasteAction.enabled = !curData._textItem.readOnly
        popup()
    }

    QtObject {
        id: curData
        property Item _textItem
    }
    Action {
        id: cutAction
        text: qsTr("&Cut")
        icon.name: "edit-cut"
        shortcut: StandardKey.Cut
        onTriggered: curData._textItem.cut()
    }
    Action {
        id: copyAction
        text: qsTr("&Copy")
        icon.name: "edit-copy"
        shortcut: StandardKey.Copy
        onTriggered: curData._textItem.copy()
    }
    Action {
        id: pasteAction
        text: qsTr("&Paste")
        icon.name: "edit-paste"
        shortcut: StandardKey.Paste
        onTriggered: curData._textItem.paste()
    }
    Action {
        id: selectAllAction
        text: qsTr("&Select all")
        icon.name: "edit-selectall"
        shortcut: StandardKey.SelectAll
        onTriggered: curData._textItem.selectAll()
    }
}
