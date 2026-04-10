import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string label:        ""
    property string description:  ""
    property real   from:         0
    property real   to:           1
    property real   stepSize:     0.1

    // Bind this to your settings property. The slider reads it on load
    // and writes back via onValueChanged when the user drags.
    property real   value:        from

    readonly property real currentValue: _value

    Layout.fillWidth: true
    implicitHeight: 52

    // Internal — starts as a copy of value, then driven by mouse
    property real _value: root.value

    // If value changes externally (e.g. persistence load), sync in
    onValueChanged: { _value = value }

    function _snap(raw) {
        if (stepSize > 0)
            return Math.max(from, Math.min(to, Math.round(raw / stepSize) * stepSize))
        return Math.max(from, Math.min(to, raw))
    }

    Row {
        anchors.fill: parent
        spacing: 16

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

        Item {
            id: track
            width:  root.width - 160 - 50 - 32
            height: 32
            anchors.verticalCenter: parent.verticalCenter

            readonly property real range: width - knob.width
            readonly property real norm: range > 0
                ? Math.max(0, Math.min(1, (root._value - root.from) / (root.to - root.from)))
                : 0

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width; height: 4; radius: 2
                color: "#1e1e2a"

                Rectangle {
                    x:      knob.width / 2
                    width:  Math.max(0, track.norm * track.range)
                    height: parent.height; radius: parent.radius
                    color:  "#7c6af7"
                }
            }

            Rectangle {
                id: knob
                width: 16; height: 16; radius: 8
                anchors.verticalCenter: parent.verticalCenter
                x: track.norm * track.range
                color: drag.pressed || drag.containsMouse ? "#c0b0ff" : "#a89ff7"
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            MouseArea {
                id: drag
                anchors.fill: parent
                hoverEnabled: true
                preventStealing: true

                function valueAt(mx) {
                    var ratio = Math.max(0, Math.min(1, (mx - knob.width / 2) / track.range))
                    return root._snap(root.from + ratio * (root.to - root.from))
                }

                onPressed:         (e) => { root._value = valueAt(e.x) }
                onPositionChanged: (e) => { if (pressed) root._value = valueAt(e.x) }
                onReleased:        root.value = root._value
            }
        }

        Rectangle {
            width: 50; height: 26; radius: 5
            anchors.verticalCenter: parent.verticalCenter
            color: "#1c1c28"; border.color: "#2a2a3a"

            Text {
                anchors.centerIn: parent
                text: root._value.toFixed(1)
                font { pixelSize: 12; family: "monospace" }
                color: "#a89ff7"
            }
        }
    }
}
