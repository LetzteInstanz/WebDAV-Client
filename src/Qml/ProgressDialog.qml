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
            property Component msgBoxComponent
            Component.onCompleted: msgBoxComponent = Qt.createComponent("CustomMessageBox.qml", Component.Asynchronous);

            function onMaxProgressEnabled(enabled) { progressBar.indeterminate = !enabled }
            function onProgressChanged(value) { progressBar.value = value }
            function onMaxProgressChanged(max) { progressBar.to = max }
            function onProgressTextChanged(text) { textLabel.text = text }
            function onErrorOccurred(text) {
                function createDlg() {
                    const comp = msgBoxComponent
                    if (comp.status === Component.Error) {
                        console.error("CustomMessageBox.qml component loading failed: ", comp.errorString());
                        return;
                    }
                    const dlg = comp.createObject(appWindow, {"standardButtons": Dialog.Ok, "text": text});
                    if (dlg === null) {
                        console.error("CustomMessageBox object creation failed");
                        return;
                    }
                    dlg.accepted.connect(progressDlg.reject)
                    dlg.closed.connect(dlg.destroy)
                    dlg.open()
                }

                const comp = msgBoxComponent
                if (comp.status === Component.Ready)
                    createDlg();
                else
                    comp.statusChanged.connect(createDlg);
            }
            function onReplyGot() { close(); }
        }
    }
}
