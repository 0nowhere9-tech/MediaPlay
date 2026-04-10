import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Use Window instead of ApplicationWindow to avoid KDE Breeze QML overlay
// hijacking the type via org.kde.breeze/ApplicationWindow.qml
Window {
    id: root
    width: 1280
    height: 800
    visible: true
    title: "MediaPlayer"
    color: "#0d0d0f"

    property string currentPage: "videos"
    property var    activeVideo: null

    RowLayout {
        anchors.fill: parent
        spacing: 0

        SideBar {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            currentPage: root.currentPage
            onNavigate: page => { root.currentPage = page }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            TopBar {
                Layout.fillWidth: true
                pageTitle: {
                    switch (root.currentPage) {
                        case "player":  return "Now Playing"
                        case "sources": return "Sources"
                        default:        return "Videos"
                    }
                }
                onBackRequested: { root.currentPage = "videos" }
                showBack: root.currentPage === "player"
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: {
                    if (root.currentPage === "player")  return 1
                    if (root.currentPage === "sources") return 2
                    return 0
                }

                VideosPage {
                    model: videoModel
                    onVideoSelected: video => {
                        root.activeVideo = video
                        root.currentPage = "player"
                    }
                }

                VideoPlayer {
                    video: root.activeVideo
                }

                SourcesPage {
                    onRefreshSource: sourceId => {
                        // TODO: forward to C++ ProviderRegistry to re-fetch
                        console.log("Refresh requested for source:", sourceId)
                    }
                }
            }
        }
    }
}
