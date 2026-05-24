import QtQuick
import QtQuick.Controls

ProgressBar {
    id: control

    property color accentColor: "#2fd27f"
    property color trackColor: "#202835"

    implicitHeight: 14

    background: Rectangle {
        radius: height / 2
        color: control.trackColor
        border.width: 1
        border.color: "#273142"
        antialiasing: true
    }

    contentItem: Item {
        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: Math.max(0, control.visualPosition * parent.width)
            height: 8
            radius: 4
            color: control.accentColor
            antialiasing: true
        }
    }
}
