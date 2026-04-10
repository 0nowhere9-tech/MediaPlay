import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property var model: null
    signal videoSelected(var video)

    // ── Search / filter bar ───────────────────────────────────────────────
    Rectangle {
        id: filterBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 56
        color: "transparent"

        Row {
            anchors { left: parent.left; leftMargin: 24; verticalCenter: parent.verticalCenter }
            spacing: 10

            // Placeholder for search input (wire up to proxy model later)
            Rectangle {
                width: 260; height: 34; radius: 6
                color: "#16161e"
                border.color: "#222230"

                Text {
                    anchors { left: parent.left; leftMargin: 12; verticalCenter: parent.verticalCenter }
                    text: "Search videos…"
                    font.pixelSize: 13
                    color: "#44445a"
                }
            }
        }
    }

    // ── Grid ──────────────────────────────────────────────────────────────
    GridView {
        id: grid
        anchors {
            top: filterBar.bottom; left: parent.left; right: parent.right; bottom: parent.bottom
            margins: 20
        }
        clip: true
        model: root.model

        cellWidth:  Math.floor(grid.width / Math.max(1, Math.floor(grid.width / 320)))
        cellHeight: cellWidth * 0.72

        // Disable built-in wheel scrolling — handled below via WheelHandler
        // so we can apply the speed multiplier from Settings
        interactive: true
        flickDeceleration: 2500

        WheelHandler {
            id: wheelHandler
            onWheel: (event) => {
                var delta = event.angleDelta.y / 120        // steps (usually ±1)
                var pixels = delta * 120 * SettingsStore.videos.scrollSpeed
                grid.contentY = Math.max(
                    grid.originY,
                    Math.min(grid.originY + grid.contentHeight - grid.height,
                             grid.contentY - pixels))
            }
        }

        delegate: VideoCard {
            width:  grid.cellWidth  - 16
            height: grid.cellHeight - 16
            x: 8; y: 8

            title:        model.title
            thumbnailUrl: model.thumbnailUrl
            duration:     model.duration
            views:        model.views
            ratio:        model.ratio
            author:       model.author
            date:         model.date
            source:       model.source

            onClicked: root.videoSelected({
                title:        model.title,
                videoUrl:     model.videoUrl,
                thumbnailUrl: model.thumbnailUrl,
                description:  model.description,
                author:       model.author,
                duration:     model.duration,
                views:        model.views,
                likes:        model.likes,
                dislikes:     model.dislikes,
                ratio:        model.ratio,
                date:         model.date,
                source:       model.source,
                tags:         model.tags,
            })
        }

        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
    }

    // ── Empty state ───────────────────────────────────────────────────────
    Column {
        visible: root.model ? root.model.count === 0 : true
        anchors.centerIn: parent
        spacing: 12

        Text { anchors.horizontalCenter: parent.horizontalCenter; text: "▶"; font.pixelSize: 48; color: "#333344" }
        Text { text: "No videos loaded"; font.pixelSize: 16; color: "#444460" }
    }
}
