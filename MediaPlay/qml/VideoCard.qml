import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    radius: 8
    color: root.hovered ? "#17171f" : "#121218"
    clip: true

    property string title:        ""
    property string thumbnailUrl: ""
    property string duration:     ""
    property int    views:        0
    property int    ratio:        0
    property string author:       ""
    property string date:         ""
    property string source:       ""

    // Declared here so child items can bind without a forward-reference warning.
    // hov is defined at the bottom of this file; the binding is resolved lazily.
    readonly property bool hovered: hov.containsMouse

    signal clicked

    border.color: root.hovered ? "#2a2a3a" : "#1a1a24"
    border.width: 1

    Behavior on color { ColorAnimation { duration: 120 } }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.62

            Image {
                id: thumb
                anchors.fill: parent
                source: root.thumbnailUrl
                fillMode: Image.PreserveAspectCrop
                smooth: true
                clip: true

                Rectangle {
                    visible: thumb.status !== Image.Ready
                    anchors.fill: parent
                    color: "#0e0e16"
                    Text {
                        anchors.centerIn: parent
                        text: "▶"
                        font.pixelSize: 28
                        color: "#2a2a3a"
                    }
                }
            }

            Rectangle {
                anchors { right: parent.right; bottom: parent.bottom; margins: 6 }
                color: "#000000cc"
                radius: 3
                width: durText.width + 10; height: 20
                Text {
                    id: durText
                    anchors.centerIn: parent
                    text: root.duration
                    font { pixelSize: 11; weight: 500 }
                    color: "#ffffff"
                }
            }

            Rectangle {
                anchors { left: parent.left; top: parent.top; margins: 6 }
                color: "#7c6af7cc"
                radius: 3
                width: srcText.width + 8; height: 18
                visible: root.source.length > 0
                Text {
                    id: srcText
                    anchors.centerIn: parent
                    text: root.source
                    font { pixelSize: 9; weight: 700; letterSpacing: 1 }
                    color: "#ffffff"
                }
            }

            Rectangle {
                anchors.fill: parent
                color: "#00000055"
                visible: root.hovered
                Text {
                    anchors.centerIn: parent
                    text: "▶"
                    font.pixelSize: 32
                    color: "#ffffffcc"
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10
            spacing: 4

            Text {
                Layout.fillWidth: true
                text: root.title
                font { pixelSize: 13; weight: 500 }
                color: "#d8d8e8"
                wrapMode: Text.Wrap
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            Item { Layout.fillHeight: true }

            Row {
                spacing: 6
                Text { text: root.author; font.pixelSize: 11; color: "#7c6af7" }
                Text { text: "·";         font.pixelSize: 11; color: "#33334a" }
                Text { text: root.date;   font.pixelSize: 11; color: "#44445a" }
            }

            Row {
                spacing: 10
                StatChip { icon: "👁"; value: root.views }
                StatChip { icon: "👍"; value: root.ratio + "%" }
            }
        }
    }

    HoverHandler { id: hov }
    TapHandler   { onTapped: root.clicked() }

    scale: root.hovered ? 1.02 : 1.0
    Behavior on scale { NumberAnimation { duration: 100 } }
}
