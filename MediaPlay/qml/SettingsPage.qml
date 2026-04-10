import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// SettingsPage — displays one SettingsSection per app page/feature.
//
// To add settings for a new page:
//   1. Add properties to SettingsStore for that page's group
//   2. Add a new SettingsSection block below — that's the only change needed

Item {
    id: root

    Flickable {
        anchors.fill: parent
        contentHeight: col.height + 48
        clip: true

        ColumnLayout {
            id: col
            anchors {
                top:   parent.top;   topMargin:   32
                left:  parent.left;  leftMargin:  32
                right: parent.right; rightMargin: 32
            }
            spacing: 12

            // ── Page header ───────────────────────────────────────
            Text {
                text: "Settings"
                font { pixelSize: 22; weight: 700; letterSpacing: 1.0 }
                color: "#e0e0e0"
            }
            Text {
                text: "Configure MediaPlayer to your preferences."
                font.pixelSize: 13
                color: "#55556a"
                bottomPadding: 8
            }

            // ── Videos section ────────────────────────────────────
            // Add more SettingsSections below as new pages are introduced.
            SettingsSection {
                title: "Videos"
                icon:  "▶"

                SettingSlider {
                    label:        "Scroll speed"
                    description:  "Mouse wheel sensitivity on the videos grid"
                    from:         1.0
                    to:           10.0
                    stepSize:     0.1
                    value:        SettingsStore.videos.scrollSpeed
                    onValueChanged: val => SettingsStore.videos.scrollSpeed = val
                }
            }

            // ── Future sections go here ───────────────────────────
            // SettingsSection { title: "Player"; icon: "⏯" ... }
            // SettingsSection { title: "Sources"; icon: "⊞" ... }
        }

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
    }
}
