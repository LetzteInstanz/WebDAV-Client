import QtQuick
import QtQuick.Layouts

BorderRectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true

    ListView {
        anchors.fill: parent
        model: fileItemModel
        clip: true
        anchors.margins: 2
        spacing: 5

        delegate: BorderRectangle {
            height: (Math.max(fileImage.height, nameText.contentHeight + dateTimeText.contentHeight) + fileItemDelegateRowLayout.anchors.topMargin + fileItemDelegateRowLayout.anchors.bottomMargin)
            width: ListView.view.width
            required property string name
            required property string extension
            required property string datetime

            RowLayout {
                id: fileItemDelegateRowLayout
                anchors.fill: parent
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.topMargin: 2

                Image {
                    id: fileImage
                    source: "image://icons/" + extension
                }
                ColumnLayout {
                    Text {
                        id: nameText
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.verticalStretchFactor: 1
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        font.bold: true
                        font.pointSize: 14
                        wrapMode: Text.Wrap
                        text: name;
                    }
                    Text {
                        id: dateTimeText
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.preferredHeight: 16
                        verticalAlignment: Text.AlignBottom
                        horizontalAlignment: Text.AlignRight
                        text: datetime;
                    }
                }
            }
        }
    }
}
