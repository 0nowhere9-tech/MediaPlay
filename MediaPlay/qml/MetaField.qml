import QtQuick

Row {
    property string label: ""
    property string value: ""
    spacing: 8

    Text {
        text: parent.label + ":"
        font.pixelSize: 12
        color: "#44445a"
        width: 70
    }
    Text {
        text: parent.value
        font.pixelSize: 12
        color: "#8888a8"
    }
}
