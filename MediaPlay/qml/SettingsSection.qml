import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    property string title:    ""
    property string icon:     ""
    property bool   expanded: true

    default property alias contents: innerCol.children

    Layout.fillWidth: true
    height: header.height + (expanded ? contentArea.implicitHeight : 0)
    radius: 8
    color:  "#111116"
    border.color: "#1e1e28"
    border.width: 1
    clip: true

    Behavior on height { NumberAnimation { duration: 180; easing.type: Easing.OutQuad } }

    // ── Header — its own Item so tap/hover stay scoped here only ──
    Item {
        id: header
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 52

        // Background tint on hover
        Rectangle {
            anchors.fill: parent
            color: hdrHov.containsMouse ? "#ffffff08" : "transparent"
            radius: root.expanded ? 0 : root.radius
            Behavior on color { ColorAnimation { duration: 120 } }
        }

        RowLayout {
            anchors { fill: parent; leftMargin: 20; rightMargin: 18 }
            spacing: 12

            Text {
                text: root.icon
                font.pixelSize: 15
                color: "#7c6af7"
            }

            Text {
                text: root.title
                font { pixelSize: 14; weight: Font.SemiBold }
                color: "#e0e0e0"
                Layout.fillWidth: true
            }

            Text {
                text: root.expanded ? "▾" : "▸"
                font.pixelSize: 13
                color: "#44445a"
            }
        }

        // Handlers scoped strictly inside the header Item
        HoverHandler { id: hdrHov }
        TapHandler   { onTapped: root.expanded = !root.expanded }
    }

    // ── Content area — completely separate from the header ────────
    Item {
        id: contentArea
        anchors { top: header.bottom; left: parent.left; right: parent.right }
        implicitHeight: innerCol.implicitHeight + 16
        clip: true

        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 1; color: "#1a1a24"
        }

        ColumnLayout {
            id: innerCol
            anchors {
                top: parent.top; topMargin: 8
                left: parent.left; leftMargin: 20
                right: parent.right; rightMargin: 20
                bottom: parent.bottom; bottomMargin: 8
            }
            spacing: 4
        }
    }
}
