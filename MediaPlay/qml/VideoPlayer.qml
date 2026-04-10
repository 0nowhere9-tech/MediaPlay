import QtQuick
import QtQuick.Layouts
import QtMultimedia

Item {
    id: root

    property var video: null

    // Guard: nothing loaded yet
    Rectangle {
        anchors.fill: parent
        color: "#0d0d0f"
        visible: !root.video

        Column {
            anchors.centerIn: parent
            spacing: 14
            Text { anchors.horizontalCenter: parent.horizontalCenter; text: "▶"; font.pixelSize: 56; color: "#222233" }
            Text { text: "Select a video to play"; font.pixelSize: 15; color: "#33334a" }
        }
    }

    // Main player layout
    RowLayout {
        anchors.fill: parent
        spacing: 0
        visible: root.video !== null

        // Left: video + controls
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#000000"

                MediaPlayer {
                    id: player
                    source: root.video ? root.video.videoUrl : ""
                    videoOutput: videoOut
                    audioOutput: AudioOutput {}
                }

                VideoOutput {
                    id: videoOut
                    anchors.fill: parent
                }

                Image {
                    anchors.fill: parent
                    source: root.video ? root.video.thumbnailUrl : ""
                    fillMode: Image.PreserveAspectFit
                    visible: player.playbackState === MediaPlayer.StoppedState
                    smooth: true
                }

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"

                    TapHandler {
                        onTapped: {
                            if (player.playbackState === MediaPlayer.PlayingState)
                                player.pause()
                            else
                                player.play()
                        }
                    }
                }
            }

            PlayerControls {
                Layout.fillWidth: true
                player: player
            }
        }

        // Right: metadata panel
        Rectangle {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            color: "#111116"
            clip: true

            Flickable {
                anchors.fill: parent
                contentHeight: metaCol.height + 32
                clip: true

                ColumnLayout {
                    id: metaCol
                    anchors { top: parent.top; left: parent.left; right: parent.right; margins: 20 }
                    spacing: 14

                    Text {
                        Layout.fillWidth: true
                        text: root.video ? root.video.title : ""
                        font { pixelSize: 15; weight: 600 }
                        color: "#e0e0e0"
                        wrapMode: Text.Wrap
                    }

                    Row {
                        spacing: 8
                        Rectangle {
                            width: srcLbl.width + 10; height: 20; radius: 3
                            color: "#7c6af744"
                            Text {
                                id: srcLbl
                                anchors.centerIn: parent
                                text: root.video ? root.video.source : ""
                                font.pixelSize: 10
                                color: "#9b8ff7"
                            }
                        }
                        Text {
                            text: root.video ? root.video.author : ""
                            font.pixelSize: 12
                            color: "#555570"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Row {
                        spacing: 16
                        MetaStat { label: "Views";    value: root.video ? root.video.views    : 0 }
                        MetaStat { label: "Likes";    value: root.video ? root.video.likes    : 0 }
                        MetaStat { label: "Dislikes"; value: root.video ? root.video.dislikes : 0 }
                    }

                    Column {
                        Layout.fillWidth: true
                        spacing: 5
                        Text { text: "Approval"; font.pixelSize: 11; color: "#44445a" }
                        Rectangle {
                            width: metaCol.width; height: 6; radius: 3; color: "#1e1e2a"
                            Rectangle {
                                width: parent.width * ((root.video ? root.video.ratio : 0) / 100)
                                height: parent.height; radius: parent.radius
                                color: "#7c6af7"
                                Behavior on width { NumberAnimation { duration: 400 } }
                            }
                        }
                        Text {
                            text: (root.video ? root.video.ratio : 0) + "%"
                            font.pixelSize: 11
                            color: "#7c6af7"
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#1a1a24" }

                    MetaField { label: "Published"; value: root.video ? root.video.date     : "" }
                    MetaField { label: "Duration";  value: root.video ? root.video.duration : "" }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#1a1a24" }

                    Text {
                        text: "Description"
                        font { pixelSize: 11; weight: 500 }
                        color: "#44445a"
                    }
                    Text {
                        Layout.fillWidth: true
                        text: root.video ? root.video.description : ""
                        font.pixelSize: 12
                        color: "#666680"
                        wrapMode: Text.Wrap
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#1a1a24" }

                    Text {
                        text: "Tags"
                        font { pixelSize: 11; weight: 500 }
                        color: "#44445a"
                    }
                    Flow {
                        Layout.fillWidth: true
                        spacing: 6

                        Repeater {
                            model: root.video ? root.video.tags : []
                            delegate: Rectangle {
                                width: tagText.width + 12; height: 22; radius: 4
                                color: "#1c1c28"
                                border.color: "#2a2a3a"
                                Text {
                                    id: tagText
                                    anchors.centerIn: parent
                                    text: modelData
                                    font.pixelSize: 11
                                    color: "#7070a0"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    onVideoChanged: {
        player.stop()
        if (root.video && root.video.videoUrl)
            player.source = root.video.videoUrl
    }
}
