import QtQuick
import QtQuick.Layouts

// SourceRow — single source entry in SourcesPage.
// Props: sourceLabel, sourceDescription, sourceStatus, sourceProgress (0-100)
// Signal: refreshClicked

Rectangle {
    id: root

    property string sourceLabel:       ""
    property string sourceDescription: ""
    property string sourceStatus:      "idle"  // "idle" | "refreshing" | "ok" | "error"
    property int    sourceProgress:    0        // 0-100, only relevant while refreshing

    signal refreshClicked

    height: sourceStatus === "refreshing" ? 76 : 64
    radius: 6
    color: "#111116"
    clip: true

    Behavior on height { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

    // Subtle border
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border {
            color: root.sourceStatus === "error" ? "#5a2222" :
                   root.sourceStatus === "ok"    ? "#1e3a2a" : "#222228"
            width: 1
        }
        Behavior on border.color { ColorAnimation { duration: 200 } }
    }

    ColumnLayout {
        anchors { fill: parent; margins: 0 }
        spacing: 0

        // Main row
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            Layout.leftMargin: 20
            Layout.rightMargin: 16
            spacing: 16

            // Status dot
            Rectangle {
                width: 8; height: 8
                radius: 4
                color: {
                    switch (root.sourceStatus) {
                        case "ok":         return "#4caf82"
                        case "refreshing": return "#f0a832"
                        case "error":      return "#e05555"
                        default:           return "#444458"
                    }
                }
                Behavior on color { ColorAnimation { duration: 200 } }

                SequentialAnimation on opacity {
                    running: root.sourceStatus === "refreshing"
                    loops:   Animation.Infinite
                    NumberAnimation { to: 0.2; duration: 600; easing.type: Easing.InOutSine }
                    NumberAnimation { to: 1.0; duration: 600; easing.type: Easing.InOutSine }
                }
            }

            // Labels
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: root.sourceLabel
                    font { pixelSize: 14; weight: 500 }
                    color: "#e0e0e0"
                }
                Text {
                    text: root.sourceStatus === "refreshing"
                          ? root.sourceProgress + "% — fetching videos…"
                          : root.sourceDescription
                    font.pixelSize: 12
                    color: root.sourceStatus === "refreshing" ? "#f0a832" : "#55556a"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }

            // Refresh button
            Rectangle {
                id: refreshBtn
                width: 96; height: 32
                radius: 5
                color: btnHov.containsMouse ? "#26263a" : "#1c1c28"
                Behavior on color { ColorAnimation { duration: 120 } }

                Text {
                    anchors.centerIn: parent
                    text: root.sourceStatus === "refreshing" ? "Cancel" : "⟳  Refresh"
                    font { pixelSize: 12; weight: 500 }
                    color: root.sourceStatus === "refreshing" ? "#888" : "#7c6af7"
                }

                HoverHandler { id: btnHov }
                TapHandler {
                    onTapped: {
                        if (root.sourceStatus === "refreshing")
                            extractionManager.cancel()
                        else
                            root.refreshClicked()
                    }
                }
            }
        }

        // Progress bar — slides in when refreshing
        Item {
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            height: root.sourceStatus === "refreshing" ? 12 : 0
            visible: root.sourceStatus === "refreshing"

            Rectangle {
                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
                height: 3
                radius: 2
                color: "#222230"

                Rectangle {
                    width: parent.width * (root.sourceProgress / 100)
                    height: parent.height
                    radius: parent.radius
                    color: "#7c6af7"
                    Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
                }
            }
        }
    }
}
