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
    function desc() { return descTxtField.text.trim() }
    function addr() {
        var addr = addressTxtField.text
        const prefix = "http://"
        if (addr.startsWith(prefix))
            addr = addr.substring(prefix.length)

        addr = addr.trim()
        return addr.endsWith("/") ? addr.substring(0, addr.length - 1) : addr
    }
    function port() { return portSpinBox.value }
    function path() {
        var path = pathTxtField.text
        if (path.startsWith("/"))
            path = path.substring(1)

        return path.endsWith("/") ? path.substring(0, path.length - 1) : path
    }
    function setData(desc, addr, port, path) {
        curData._desc = desc; curData._addr = addr; curData._port = port; curData._path = path
        descTxtField.text = desc; addressTxtField.text = addr; portSpinBox.value = port; pathTxtField.text = path
        curData.enableOkButton()
    }
    function enableHasChangesFunc(enable) { curData._enabledHasChangesFunc = enable }

    QtObject {
        id: curData
        property bool _enabledHasChangesFunc: false
        property string _desc
        property string _addr
        property int _port
        property string _path

        function isFieldEmpty() { return desc() === "" || addr() === "" }
        function hasChanges() { return !_enabledHasChangesFunc || (_desc !== desc() || _addr !== addr() || _port !== portSpinBox.value || _path !== path()) }
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
                onReleased: (event) => { textContextMenu.hanldeReleaseEvent(descTxtField, event) }
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
                onReleased: (event) => { textContextMenu.hanldeReleaseEvent(addressTxtField, event) }
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
                placeholderText: qsTr("directory 1/directory 2/…")
                Layout.fillWidth: true
                onTextEdited: curData.enableOkButton()
                onReleased: (event) => { textContextMenu.hanldeReleaseEvent(pathTxtField, event) }
            }
        }
    }
}
