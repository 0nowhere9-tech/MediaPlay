import QtQuick
import QtQuick.Layouts

// SourcesPage
// Displays all registered video sources with per-source refresh controls.
// Reads extractionManager properties directly (context property from C++):
//   extractionManager.running, .progress, .phase, .statusText, .eta

Item {
    id: root

    property var sources: [
        {
            id:          "hentaicity",
            label:       "Hentai City",
            description: "Streams from HentaiCity · ~4,500 videos",
            status:      "idle"
        }
    ]

    Connections {
        target: extractionManager

        function onRunningChanged() {
            if (!extractionManager.running) {
                // Mark all refreshing sources as ok when the run ends
                for (var i = 0; i < root.sources.length; ++i) {
                    if (root.sources[i].status === "refreshing") {
                        var updated = root.sources.slice()
                        updated[i] = Object.assign({}, updated[i], { status: "ok" })
                        root.sources = updated
                    }
                }
            }
        }

        function onError(sourceName, message) {
            for (var i = 0; i < root.sources.length; ++i) {
                if (root.sources[i].id.toLowerCase() === sourceName.toLowerCase()) {
                    var updated = root.sources.slice()
                    updated[i] = Object.assign({}, updated[i], { status: "error" })
                    root.sources = updated
                }
            }
        }
    }

    function startRefresh(sourceId) {
        for (var i = 0; i < sources.length; ++i) {
            if (sources[i].id === sourceId) {
                var updated = sources.slice()
                updated[i] = Object.assign({}, updated[i], { status: "refreshing" })
                sources = updated
                break
            }
        }
        extractionManager.refresh(sourceId)
    }

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

                // Wire the new detail props directly from extractionManager.
                // Only meaningful while this source is the one refreshing.
                sourcePhase:      modelData.status === "refreshing" ? extractionManager.phase      : ""
                sourceProgress:   modelData.status === "refreshing" ? extractionManager.progress   : 0
                sourceStatusText: modelData.status === "refreshing" ? extractionManager.statusText : ""
                sourceEta:        modelData.status === "refreshing" ? extractionManager.eta        : ""

                onRefreshClicked: root.startRefresh(modelData.id)
            }
        }
    }
}
