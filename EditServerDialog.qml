import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    anchors.centerIn: parent
    width: parent.width - 10
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onOpened: handleEnteredTexts()

    function handleEnteredTexts() { standardButton(Dialog.Ok).enabled = descTxtField.text !== "" && addressTxtField.text !== "" }
    function desc() { return descTxtField.text }
    function setDesc(string) { descTxtField.text = string }
    function addr() { return addressTxtField.text }
    function setAddr(string) { addressTxtField.text = string }
    function port() { return portSpinBox.value }
    function setPort(integer) { portSpinBox.value = integer }
    function path() { return pathTxtField.text }
    function setPath(string) { pathTxtField.text = string }
    function resetData() {
        descTxtField.clear()
        addressTxtField.clear()
        portSpinBox.value = 80;
        pathTxtField.clear()
    }
    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Label {
                id: nameLabel
                text: qsTr("Name:")
                Layout.preferredHeight: descTxtField.height
                verticalAlignment: Text.AlignVCenter
            }
            /*Label {
                id: protocolLabel
                text: qsTr("Protocol:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: httpRadioButton.height
            }*/
            Label {
                id: serverLabel
                text: qsTr("Server address:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: addressTxtField.height
            }
            Label {
                id: portLabel
                text: qsTr("Port:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: portSpinBox.height
            }
            Label {
                id: dirLabel
                text: qsTr("Initial directory:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: pathTxtField.height
            }
        }
        ColumnLayout {
            TextField {
                id: descTxtField
                Layout.fillWidth: true
                placeholderText: qsTr("Description")
                onTextEdited: { handleEnteredTexts() }
            }
            /*RowLayout {
                RadioButton {
                    id: httpRadioButton
                    text: qsTr("HTTP")
                    enabled: false
                    checked: true
                }
                RadioButton {
                    id: httpsRadioButton
                    text: qsTr("HTTPS")
                    enabled: false
                }
            }*/
            TextField {
                id: addressTxtField
                Layout.fillWidth: true
                placeholderText: "example.com" + qsTr(" or IP address")
                onTextEdited: { handleEnteredTexts() }
            }
            SpinBox {
                id: portSpinBox
                editable: true
                from: 1
                to: 65535
            }
            TextField {
                id: pathTxtField
                placeholderText: qsTr("directory_1/directory_2/…")
                Layout.fillWidth: true
            }
        }
    }
}
