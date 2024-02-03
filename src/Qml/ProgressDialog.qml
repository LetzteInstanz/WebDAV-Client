import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: progressDlg
    anchors.centerIn: parent
    width: parent.width / 2
    modal: true
    //closePolicy: Popup.CloseOnEscape
    standardButtons: Dialog.Cancel
    onRejected: fileSystemModel.stop()

    CustomMessageBox {
        id: errorDlg
        standardButtons: Dialog.Ok
        onAccepted: progressDlg.reject()
    }
    ColumnLayout {
        anchors.fill: parent

        ProgressBar {
            id: progressBar
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            id: textLabel
            Layout.alignment: Qt.AlignHCenter
            text: "textLabel"
        }
        Connections {
            target: fileSystemModel
            function onMaxProgressEnabled(enabled) { progressBar.indeterminate = !enabled }
            function onProgressChanged(value) { progressBar.value = value }
            function onMaxProgressChanged(max) { progressBar.to = max }
            function onProgressTextChanged(text) { textLabel.text = text }
            function onErrorOccurred(text) {
                errorDlg.text = text;
                errorDlg.open()
            }
            function onReplyGot() { close(); }
        }
    }
}
