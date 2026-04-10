import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia

Rectangle {
    id: root
    height: 72
    color: "#0d0d11"

    property MediaPlayer player

    function formatTime(ms) {
        var total = Math.floor(ms / 1000)
        var h = Math.floor(total / 3600)
        var m = Math.floor((total % 3600) / 60)
        var s = total % 60
        var mm = m < 10 ? "0" + m : "" + m
        var ss = s < 10 ? "0" + s : "" + s
        if (h > 0)
            return h + ":" + mm + ":" + ss
        return m + ":" + ss
    }

    ColumnLayout {
        anchors { fill: parent; margins: 12 }
        spacing: 6

        // Seek bar
        Item {
            Layout.fillWidth: true
            height: 16

            Rectangle {
                id: track
                anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
                height: 4; radius: 2
                color: "#1e1e2a"

                Rectangle {
                    width: root.player && root.player.duration > 0
                           ? track.width * (root.player.bufferedTime / root.player.duration)
                           : 0
                    height: parent.height; radius: parent.radius
                    color: "#2a2a40"
                }

                Rectangle {
                    width: root.player && root.player.duration > 0
                           ? track.width * (root.player.position / root.player.duration)
                           : 0
                    height: parent.height; radius: parent.radius
                    color: "#7c6af7"
                }
            }

            Rectangle {
                x: root.player && root.player.duration > 0
                   ? (track.width * (root.player.position / root.player.duration)) - width / 2
                   : -width / 2
                anchors.verticalCenter: parent.verticalCenter
                width: seekHov.containsMouse ? 14 : 10
                height: width; radius: width / 2
                color: "#a89ff7"
                Behavior on width { NumberAnimation { duration: 100 } }
            }

            HoverHandler { id: seekHov }
            TapHandler {
                onTapped: function(event) {
                    if (root.player && root.player.duration > 0)
                        root.player.position = (event.position.x / track.width) * root.player.duration
                }
            }
        }

        // Controls row
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            // -10s
            Rectangle {
                width: skipBack.width + 16; height: 28; radius: 5
                color: sbHov.containsMouse ? "#1e1e2e" : "transparent"
                Text { id: skipBack; anchors.centerIn: parent; text: "⏮ 10s"; font.pixelSize: 13; color: "#7070a0" }
                HoverHandler { id: sbHov }
                TapHandler { onTapped: root.player.position = Math.max(0, root.player.position - 10000) }
            }

            // Play/pause
            Rectangle {
                width: ppLabel.width + 16; height: 28; radius: 5
                color: ppHov.containsMouse ? "#9880ff" : "#7c6af7"
                Text {
                    id: ppLabel
                    anchors.centerIn: parent
                    text: root.player && root.player.playbackState === MediaPlayer.PlayingState ? "⏸" : "▶"
                    font.pixelSize: 13; color: "#ffffff"
                }
                HoverHandler { id: ppHov }
                TapHandler {
                    onTapped: {
                        if (root.player.playbackState === MediaPlayer.PlayingState)
                            root.player.pause()
                        else
                            root.player.play()
                    }
                }
            }

            // +10s
            Rectangle {
                width: skipFwd.width + 16; height: 28; radius: 5
                color: sfHov.containsMouse ? "#1e1e2e" : "transparent"
                Text { id: skipFwd; anchors.centerIn: parent; text: "10s ⏭"; font.pixelSize: 13; color: "#7070a0" }
                HoverHandler { id: sfHov }
                TapHandler { onTapped: root.player.position = Math.min(root.player.duration, root.player.position + 10000) }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: root.player
                      ? formatTime(root.player.position) + " / " + formatTime(root.player.duration)
                      : "0:00 / 0:00"
                font { pixelSize: 12; family: "monospace" }
                color: "#666680"
            }

            Text { text: "🔊"; font.pixelSize: 13; color: "#44445a" }

            Slider {
                id: volSlider
                from: 0; to: 1; value: 0.8
                implicitWidth: 80; implicitHeight: 20
                onValueChanged: if (root.player) root.player.audioOutput.volume = value

                background: Rectangle {
                    x: volSlider.leftPadding
                    y: volSlider.topPadding + volSlider.availableHeight / 2 - height / 2
                    width: volSlider.availableWidth; height: 4; radius: 2
                    color: "#1e1e2a"
                    Rectangle {
                        width: volSlider.visualPosition * parent.width
                        height: parent.height; radius: parent.radius
                        color: "#7c6af7"
                    }
                }

                handle: Rectangle {
                    x: volSlider.leftPadding + volSlider.visualPosition * volSlider.availableWidth - width / 2
                    y: volSlider.topPadding + volSlider.availableHeight / 2 - height / 2
                    width: 12; height: 12; radius: 6
                    color: "#a89ff7"
                }
            }
        }
    }
}
