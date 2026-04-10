import QtQuick

Rectangle {
    property string icon
    property string label
    property bool   active: false
    signal clicked

    readonly property bool hovered: hov.containsMouse

    height: 52
    color: active ? "#1c1c24" : (hovered ? "#161620" : "transparent")

    // Active indicator bar
    Rectangle {
        visible: parent.active
        width: 3; height: parent.height
        anchors.left: parent.left
        color: "#7c6af7"
    }

    Row {
        anchors { left: parent.left; leftMargin: 28; verticalCenter: parent.verticalCenter }
        spacing: 14

        Text {
            text: parent.parent.icon
            font.pixelSize: 16
            color: parent.parent.active ? "#a89ff7" : "#666680"
        }
        Text {
            text: parent.parent.label
            font { pixelSize: 13; weight: parent.parent.active ? 500 : 400 }
            color: parent.parent.active ? "#e0e0e0" : "#666680"
        }
    }

    HoverHandler { id: hov }
    TapHandler   { onTapped: parent.clicked() }
}
