import QtQuick
import QtQuick.Layouts

// SourceRow — single source entry in SourcesPage.
//
// Props:
//   sourceLabel, sourceDescription, sourceStatus
//   sourcePhase       "listing" | "fetching" | ""
//   sourceProgress    0-100
//   sourceStatusText  e.g. "42 / 300 new  ·  12 skipped"
//   sourceEta         e.g. "3m 12s"
// Signal: refreshClicked

Rectangle {
    id: root

    property string sourceLabel:       ""
    property string sourceDescription: ""
    property string sourceStatus:      "idle"   // "idle" | "refreshing" | "ok" | "error"
    property string sourcePhase:       ""       // "listing" | "fetching" | ""
    property int    sourceProgress:    0
    property string sourceStatusText:  ""
    property string sourceEta:         ""

    signal refreshClicked

    height: sourceStatus === "refreshing" ? 92 : 64
    radius: 6
    color: "#111116"
    clip: true

    Behavior on height { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }

    // Border
    Rectangle {
        anchors.fill: parent; radius: parent.radius; color: "transparent"
        border {
            color: root.sourceStatus === "error" ? "#5a2222"
                 : root.sourceStatus === "ok"    ? "#1e3a2a" : "#222228"
            width: 1
        }
        Behavior on border.color { ColorAnimation { duration: 200 } }
    }

    ColumnLayout {
        anchors { fill: parent; margins: 0 }
        spacing: 0

        // ── Main row ──────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            Layout.leftMargin: 20
            Layout.rightMargin: 16
            spacing: 16

            // Status dot
            Rectangle {
                width: 8; height: 8; radius: 4
                color: root.sourceStatus === "ok"         ? "#4caf82"
                     : root.sourceStatus === "refreshing" ? "#f0a832"
                     : root.sourceStatus === "error"      ? "#e05555"
                     :                                      "#444458"
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
                spacing: 3

                Text {
                    text: root.sourceLabel
                    font { pixelSize: 14; weight: 500 }
                    color: "#e0e0e0"
                }

                Text {
                    Layout.fillWidth: true
                    text: {
                        if (root.sourceStatus !== "refreshing")
                            return root.sourceDescription
                        if (root.sourcePhase === "listing")
                            return "Step 1 / 2 — Building video list…"
                        if (root.sourcePhase === "fetching")
                            return "Step 2 / 2 — Fetching video data"
                        return "Starting…"
                    }
                    font { pixelSize: 12; weight: root.sourceStatus === "refreshing" ? 500 : 400 }
                    color: root.sourceStatus === "refreshing" ? "#f0a832" : "#55556a"
                    elide: Text.ElideRight
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }

            // Refresh / Cancel button
            Rectangle {
                width: 96; height: 32; radius: 5
                color: btnHov.containsMouse ? "#26263a" : "#1c1c28"
                Behavior on color { ColorAnimation { duration: 120 } }

                Text {
                    anchors.centerIn: parent
                    text:  root.sourceStatus === "refreshing" ? "Cancel" : "⟳  Refresh"
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

        // ── Progress section ──────────────────────────────────────
        ColumnLayout {
            visible: root.sourceStatus === "refreshing"
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 16
            Layout.bottomMargin: 10
            spacing: 5

            // Progress bar — purple during listing, green during fetching
            Rectangle {
                Layout.fillWidth: true; height: 3; radius: 2; color: "#222230"
                Rectangle {
                    width: parent.width * (root.sourceProgress / 100)
                    height: parent.height; radius: parent.radius
                    color: root.sourcePhase === "listing" ? "#7c6af7" : "#4caf82"
                    Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
                    Behavior on color { ColorAnimation  { duration: 200 } }
                }
            }

            // Detail row: status text left, ETA right
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: root.sourcePhase === "listing"
                          ? root.sourceProgress + "%  ·  " + root.sourceStatusText
                          : root.sourceStatusText
                    font.pixelSize: 11
                    color: "#666680"
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Text {
                    visible: root.sourcePhase === "fetching" && root.sourceEta !== ""
                    text: "ETA " + root.sourceEta
                    font.pixelSize: 11
                    color: "#555570"
                }
            }
        }
    }
}
