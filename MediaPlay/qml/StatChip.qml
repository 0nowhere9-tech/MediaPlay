import QtQuick

Row {
    property string icon: ""
    property var    value: ""
    spacing: 3

    Text { text: parent.icon;  font.pixelSize: 10; color: "#44445a" }
    Text { text: parent.value; font.pixelSize: 10; color: "#55556a" }
}
