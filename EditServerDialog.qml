import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    anchors.centerIn: parent
    width: parent.width - 20
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onOpened: {
        descTxtField.focus = true
        standardButton(Dialog.Ok).enabled = false
    }
    background: BorderRectangle {}
    function desc() { return descTxtField.text }
    function setDesc(string) {
        curData.desc = string
        descTxtField.text = string
    }
    function addr() { return addressTxtField.text }
    function setAddr(string) {
        curData.addr = string
        addressTxtField.text = string
    }
    function port() { return portSpinBox.value }
    function setPort(integer) {
        curData.port = integer
        portSpinBox.value = integer
    }
    function path() { return pathTxtField.text }
    function setPath(string) {
        curData.path = string
        pathTxtField.text = string
    }
    function enableHasChangesFunc(enable) { curData.enabledHasCahngesFunc = enable }
    function resetData() {
        curData.desc = ""
        curData.addr = ""
        curData.port = 80
        curData.path = ""
        descTxtField.clear()
        addressTxtField.clear()
        portSpinBox.value = 80
        pathTxtField.clear()
        curData.enableOkButton()
    }

    QtObject {
        id: curData
        property bool enabledHasCahngesFunc: false
        property string desc
        property string addr
        property int port
        property string path

        function isFieldEmpty() { return descTxtField.text === "" || addressTxtField.text === "" }
        function hasCahnges() { return !curData.enabledHasCahngesFunc || (curData.desc !== descTxtField.text || curData.addr !== addressTxtField.text || curData.port !== portSpinBox.value || curData.path !== addressTxtField) }
        function enableOkButton() { standardButton(Dialog.Ok).enabled = hasChanges() && !isFieldEmpty() }
    }
    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Label {
                text: qsTr("Name:")
                Layout.preferredHeight: descTxtField.height
                verticalAlignment: Text.AlignVCenter
            }
            /*Label {
                text: qsTr("Protocol:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: httpRadioButton.height
            }*/
            Label {
                text: qsTr("Server address:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: addressTxtField.height
            }
            Label {
                text: qsTr("Port:")
                verticalAlignment: Text.AlignVCenter
                Layout.preferredHeight: portSpinBox.height
            }
            Label {
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
                onTextEdited: curData.enableOkButton()
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
                onTextEdited: curData.enableOkButton()
            }
            SpinBox {
                id: portSpinBox
                editable: true
                from: 1
                to: 65535
                onValueModified: curData.enableOkButton()
            }
            TextField {
                id: pathTxtField
                placeholderText: qsTr("directory_1/directory_2/…")
                Layout.fillWidth: true
                onTextEdited: curData.enableOkButton()
            }
        }
    }
}
