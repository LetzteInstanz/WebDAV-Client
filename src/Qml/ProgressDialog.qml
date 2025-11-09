import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util

Dialog {
    id: progressDlg
    anchors.centerIn: parent
    width: parent.width / 2
    modal: true
    closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Cancel
    background: Core.BorderRectangle {}
    title: qsTr("Progress")
    property bool indeterminate: true
    property int value: 0
    readonly property int to: 1
    required property var fileSystemModel
    readonly property var countFunc: null
    contentItem: ColumnLayout {
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            indeterminate: progressDlg.indeterminate
            value: progressDlg.value
            to: progressDlg.to
        }
        Label {
            id: textLabel
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.Wrap
        }
    }
    QtObject {
        id: privateObj
        property int errorCount: 0

        function complete(fileSystemId) {
            ++progressBar.value
            if (progressBar.value < progressBar.to || progressBar.to === privateObj.errorCount)
                return

            accept()
        }
    }
    Connections {
        target: fileSystemModel
        function onMaxProgressEnabled(enabled) { progressBar.indeterminate = !enabled }
        function onProgressChanged(value) { progressBar.value = value }
        function onMaxProgressChanged(max) { progressBar.to = max }
        function onProgressTextChanged(text) { textLabel.text = text }
        function onErrorOccurred(fileSystemId, text, isCritical) {
            ++privateObj.errorCount
            privateObj.complete(fileSystemId)
            if (privateObj.errorCount < to && !isCritical)
                return

            function createDlg(comp) {
                const dlg = Util.createPopup(comp, progressDlg.ApplicationWindow.window, {"standardButtons": Dialog.Ok, "title": qsTr("Error"), "text": privateObj.errorCount > 1 ? qsTr("Errors occurred") : text})
                if (dlg === null)
                    return

                dlg.closed.connect(reject)
                dlg.open()
            }

            const comp = Qt.createComponent("Core/MessageBox.qml", Component.Asynchronous)
            Util.createObjAsync(comp, createDlg)
        }
        function onReady(fileSystemId) {
            if (countFunc)
                countFunc(fileSystemId)

            privateObj.complete(fileSystemId)
        }
    }
}
