import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TabButton {
    id: control

    property color accentColor: "#2fd27f"
    property color surfaceColor: "#151b24"
    property color hoverColor: "#1a2230"
    property color checkedColor: "#1c2531"
    property color borderColor: "#273241"
    property color textColor: "#93a4b4"
    property color checkedTextColor: "#eef4fb"

    hoverEnabled: true
    padding: 0
    implicitHeight: 36
    implicitWidth: 180

    background: Rectangle {
        radius: 10
        border.width: 1
        border.color: control.checked ? control.accentColor : control.borderColor
        color: control.down
            ? "#10151c"
            : control.checked
                ? control.checkedColor
                : control.hovered
                    ? control.hoverColor
                    : control.surfaceColor
        antialiasing: true
    }

    contentItem: Label {
        text: control.text
        color: control.checked ? control.checkedTextColor : control.textColor
        font.pixelSize: 13
        font.weight: Font.DemiBold
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
