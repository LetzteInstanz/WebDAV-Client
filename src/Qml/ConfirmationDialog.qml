import QtQml
import QtQuick.Controls

import "Core" as Core

Core.MessageBox {
    standardButtons: Dialog.Yes | Dialog.No
    title: qsTr("Confirmation")
    text: {
        var text = qsTr("Are you sure you want to download ")
        const hasDir = dirCount > 0
        if (hasDir)
            text += dirCount + (dirCount > 1 ? qsTr(" folders") : qsTr(" folder"))

        if (fileCount > 0) {
            if (hasDir)
                text += qsTr(" and ")

            text += fileCount + (fileCount > 1 ? qsTr(" files") : qsTr(" file"))
        }
        function getAvailabilityErrorText() { return qsTr("not all resources are available") }
        if (!invalidAllSizes) {
            text += " ("
            if (!validAllSizes)
                text += qsTr("at least ")

            text += sizeStr + ")"
        }
        text += "?"
        return text
    }
    required property int dirCount
    required property int fileCount
    required property string sizeStr
    required property bool validAllSizes
    required property bool invalidAllSizes
}
