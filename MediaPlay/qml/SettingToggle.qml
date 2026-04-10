import QtQuick
import QtQuick.Layouts

// SettingToggle — a label + description + toggle switch row.
// Matches the layout of SettingSlider so sections look consistent.
//
// Usage:
//   SettingToggle {
//       label:       "Infinite scroll"
//       description: "Automatically load more videos when reaching the bottom"
//       value:       SettingsStore.videosInfiniteScroll
//       onCommitted: val => SettingsStore.videosInfiniteScroll = val
//   }

Item {
    id: root

    property string label:       ""
    property string description: ""
    property bool   value:       false

    signal committed(bool val)

    Layout.fillWidth: true
    implicitHeight: 52

    property bool _value: root.value
    onValueChanged: _value = value

    Row {
        anchors.fill: parent
        spacing: 16

        // Label column — same fixed width as SettingSlider
        Column {
            width: 160
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Text {
                text: root.label
                width: 160
                font { pixelSize: 13; weight: Font.Medium }
                color: "#d0d0e0"
                elide: Text.ElideRight
            }
            Text {
                text:    root.description
                visible: root.description !== ""
                width:   160
                font.pixelSize: 11
                color: "#44445a"
                elide: Text.ElideRight
            }
        }

        // Spacer so toggle sits at the right side of the row
        Item {
            width:  root.width - 160 - togglePill.width - 32
            height: 1
        }

        // Toggle pill
        Rectangle {
            id: togglePill
            width: 44; height: 24; radius: 12
            anchors.verticalCenter: parent.verticalCenter
            color: root._value ? "#7c6af7" : "#2a2a3a"

            Behavior on color { ColorAnimation { duration: 150 } }

            Rectangle {
                id: thumb
                width: 18; height: 18; radius: 9
                anchors.verticalCenter: parent.verticalCenter
                x: root._value ? parent.width - width - 3 : 3
                color: root._value ? "#ffffff" : "#888899"

                Behavior on x     { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
                Behavior on color { ColorAnimation   { duration: 150 } }
            }

            HoverHandler { id: togHov }
            TapHandler {
                onTapped: {
                    root._value = !root._value
                    root.value  = root._value
                    root.committed(root._value)
                }
            }
        }
    }
}
