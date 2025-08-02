import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import "Core" as Core
import "Util.js" as Util
import WebDavClient

ColumnLayout {
    function prepare() {
        askPathCheckBox.checkState = Settings.getAskPathFlag() ? Qt.Checked : Qt.Unchecked
        pathTxtField.text = Settings.getDownloadPath()
        logLevelComboBox.currentIndex = Settings.getCurrentLogLevel()
        saveSettingsButton.enabled = false
    }
    function back() { stackLayout.currentIndex = 0 }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: saveSettingsButton.height

        Core.Button {
            text: qsTr("Back")
            onClicked: back()
        }
        Core.Button {
            id: saveSettingsButton
            anchors.right: parent.right
            text: qsTr("Ok")
            onClicked: {
                Settings.setAskPathFlag(askPathCheckBox.checkState === Qt.Checked)
                Settings.setDownloadPath(pathTxtField.text)
                Settings.setCurrentLogLevel(logLevelComboBox.currentIndex)
                back()
            }
        }
    }
    Core.BorderRectangle {
        Layout.fillHeight: true
        Layout.fillWidth: true

        Column {
            id: settingsColumnLayout
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5
            function hasChanges() { return Settings.getAskPathFlag() !== (askPathCheckBox.checkState === Qt.Checked) || Settings.getDownloadPath() !== pathTxtField.text || Settings.getCurrentLogLevel() !== logLevelComboBox.currentIndex }

            GroupBox {
                width: parent.width
                title: qsTr("Download path")

                Column {
                    anchors.fill: parent

                    CheckBox {
                        id: askPathCheckBox
                        leftPadding: 0 // todo: check on a cell
                        text: qsTr("Always ask")
                        onClicked: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
                    }
                    Label {
                        id: pathLabel
                        text: qsTr("Path:")
                    }
                    Row {
                        width: parent.width
                        spacing: 5

                        TextField {
                            id: pathTxtField
                            width: parent.width - parent.spacing - pathButton.width
                            readOnly: true
                            onTextChanged: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
                        }
                        Button {
                            id: pathButton
                            text: qsTr("Select")
                            onClicked: {
                                function createDlg(comp) {
                                    const dlg = Util.createDialog(folderDlgComp, parent, {"currentFolder": Settings.addScheme(pathTxtField.text)})
                                    if (dlg === null)
                                        return

                                    function setPath() {
                                        const path = Settings.removeScheme(dlg.selectedFolder)
                                        if (path === "") {
                                            console.error(qsTr("QML: Invalid URI: " + dlg.selectedFolder))
                                            return;
                                        }
                                        console.debug(qsTr("QML: The download path is set: " + path))
                                        pathTxtField.text = path
                                    }
                                    dlg.accepted.connect(setPath)
                                    dlg.open()
                                }

                                Util.createObjAsync(folderDlgComp, createDlg)
                            }

                            Component {
                                id: folderDlgComp
                                FolderDialog {}
                            }
                        }
                    }
                }
            }
            Label {
                text: qsTr("Maximum log level:")
            }
            ComboBox {
                id: logLevelComboBox
                model: Settings.getLevelDescList()
                delegate: ItemDelegate {
                    text: modelData
                    required property string modelData
                }
                onActivated: saveSettingsButton.enabled = settingsColumnLayout.hasChanges()
            }
        }
    }
}
