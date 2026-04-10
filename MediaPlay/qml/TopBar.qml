import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    height: 60
    color: "#0d0d0f"

    property string pageTitle: ""
    property bool   showBack:  false
    signal backRequested

    // Bottom border
    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#1a1a22" }

    RowLayout {
        anchors { fill: parent; leftMargin: 24; rightMargin: 24 }
        spacing: 12

        // Back button
        Rectangle {
            visible: root.showBack
            width: 32; height: 32; radius: 6
            color: backHov.containsMouse ? "#222230" : "transparent"

            Text { anchors.centerIn: parent; text: "←"; font.pixelSize: 18; color: "#9090b0" }
            HoverHandler { id: backHov }
            TapHandler   { onTapped: root.backRequested() }
        }

        Text {
            text: root.pageTitle
            font { pixelSize: 18; weight: 600; letterSpacing: 1 }
            color: "#e0e0e0"
        }

        Item { Layout.fillWidth: true }

        // Source badge / info area (expandable later)
        Text {
            text: "All Sources"
            font.pixelSize: 12
            color: "#444460"
        }
    }
}
