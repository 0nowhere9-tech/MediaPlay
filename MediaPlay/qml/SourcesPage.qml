import QtQuick
import QtQuick.Layouts

// SourcesPage
// Displays all registered video sources with per-source refresh buttons.
// Reads extractionManager.running and extractionManager.progress (context props)
// to reflect live extraction state in the UI.
//
// To add a new source: append an entry to the `sources` model below,
// matching the id to the extractor's sourceName() (case-insensitive).

Item {
    id: root

    // Source registry — mirrors ExtractorRegistry at the UI level.
    // status is driven by extractionManager signals below.
    property var sources: [
        {
            id:          "hentaicity",
            label:       "Hentai City",
            description: "Streams from HentaiCity · ~4,500 videos",
            status:      "idle",
            progress:    0
        }
    ]

    // Keep source status in sync with extractionManager signals.
    Connections {
        target: extractionManager

        function onRunningChanged() {
            if (!extractionManager.running) {
                // Run finished — mark all refreshing sources as ok
                for (var i = 0; i < root.sources.length; ++i) {
                    if (root.sources[i].status === "refreshing") {
                        var updated = root.sources.slice()
                        updated[i] = Object.assign({}, updated[i], { status: "ok", progress: 100 })
                        root.sources = updated
                    }
                }
            }
        }

        function onProgressChanged() {
            for (var i = 0; i < root.sources.length; ++i) {
                if (root.sources[i].status === "refreshing") {
                    var updated = root.sources.slice()
                    updated[i] = Object.assign({}, updated[i],
                        { progress: extractionManager.progress })
                    root.sources = updated
                }
            }
        }

        function onError(sourceName, message) {
            for (var i = 0; i < root.sources.length; ++i) {
                if (root.sources[i].id.toLowerCase() === sourceName.toLowerCase()) {
                    var updated = root.sources.slice()
                    updated[i] = Object.assign({}, updated[i], { status: "error", progress: 0 })
                    root.sources = updated
                }
            }
        }
    }

    function startRefresh(sourceId) {
        for (var i = 0; i < sources.length; ++i) {
            if (sources[i].id === sourceId) {
                var updated = sources.slice()
                updated[i] = Object.assign({}, updated[i], { status: "refreshing", progress: 0 })
                sources = updated
                break
            }
        }
        extractionManager.refresh(sourceId)
    }

    // ---------------------------------------------------------------------------
    // Layout
    // ---------------------------------------------------------------------------
    ColumnLayout {
        anchors { top: parent.top; left: parent.left; right: parent.right
                  topMargin: 32; leftMargin: 32; rightMargin: 32 }
        spacing: 8

        Text {
            text: "Sources"
            font { pixelSize: 22; weight: 700; letterSpacing: 1 }
            color: "#e0e0e0"
        }

        Text {
            text: "Manage the video sources available to MediaPlayer."
            font.pixelSize: 13
            color: "#55556a"
            bottomPadding: 16
        }

        Repeater {
            model: root.sources
            delegate: SourceRow {
                Layout.fillWidth: true
                sourceLabel:       modelData.label
                sourceDescription: modelData.description
                sourceStatus:      modelData.status
                sourceProgress:    modelData.progress
                onRefreshClicked:  root.startRefresh(modelData.id)
            }
        }
    }
}
