import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util

Dialog {
    id: mainDialog
    anchors.centerIn: parent
    width: parent.width / 2
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Cancel
    background: Core.BorderRectangle {}
    title: qsTr("Progress")
    contentItem: ColumnLayout {
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            id: textLabel
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
        }
        Connections {
            target: FileSystemModel
            function onMaxProgressEnabled(enabled) { progressBar.indeterminate = !enabled }
            function onProgressChanged(value) { progressBar.value = value }
            function onMaxProgressChanged(max) { progressBar.to = max }
            function onProgressTextChanged(text) { textLabel.text = text }
            function onErrorOccurred(text) {
                console.debug(qsTr("QML: An error occurred"))

                function createDlg(comp) {
                    const dlg = Util.createPopup(comp, mainDialog.ApplicationWindow.window, {"standardButtons": Dialog.Ok, "title": qsTr("Error"), "text": text})
                    if (dlg === null)
                        return

                    dlg.closed.connect(reject)
                    dlg.open()
                }

                var comp = Qt.createComponent("Core/MessageBox.qml", Component.Asynchronous)
                Util.createObjAsync(comp, createDlg)
            }
            function onReplyGot() { console.debug(qsTr("QML: A reply was received")); accept() }
        }
    }
}
