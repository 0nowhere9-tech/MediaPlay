import QtQuick

Column {
    property string label: ""
    property var    value: 0
    spacing: 2

    Text {
        text: parent.value
        font { pixelSize: 16; weight: 600 }
        color: "#d0d0e8"
    }
    Text {
        text: parent.label
        font.pixelSize: 10
        color: "#44445a"
    }
}
