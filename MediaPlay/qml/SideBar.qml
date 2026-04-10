import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#111116"

    property string currentPage: "videos"
    signal navigate(string page)

    readonly property var navItems: [
        { id: "videos",   icon: "▶", label: "Videos"   },
        { id: "settings", icon: "⚙", label: "Settings" },
    ]

    // Top nav items
    ColumnLayout {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        spacing: 0

        Item {
            Layout.fillWidth: true
            height: 72
            Text {
                anchors.centerIn: parent
                text: "MEDIAPLAY"
                font { pixelSize: 15; letterSpacing: 6; weight: 700 }
                color: "#e0e0e0"
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#222228" }

        Repeater {
            model: root.navItems
            delegate: NavItem {
                Layout.fillWidth: true
                icon:    modelData.icon
                label:   modelData.label
                active:  root.currentPage === modelData.id
                onClicked: root.navigate(modelData.id)
            }
        }
    }

    // Sources button — pinned to the bottom of the sidebar
    ColumnLayout {
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        spacing: 0

        Rectangle { Layout.fillWidth: true; height: 1; color: "#222228" }

        NavItem {
            Layout.fillWidth: true
            icon:    "⊞"
            label:   "Sources"
            active:  root.currentPage === "sources"
            onClicked: root.navigate("sources")
        }
    }
}
