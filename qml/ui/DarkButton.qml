import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Button {
    id: control

    property color accentColor: "#2fd27f"
    property color surfaceColor: "#171d26"
    property color hoverColor: "#1d2530"
    property color pressedColor: "#11161d"
    property color borderColor: "#283244"
    property color checkedSurfaceColor: "#1b2430"
    property color textColor: "#edf3f8"
    property color checkedTextColor: "#f5fff9"
    property bool compact: false
    property bool rounded: true

    hoverEnabled: true
    padding: 0
    display: AbstractButton.TextBesideIcon
    implicitHeight: compact ? 30 : 38
    implicitWidth: compact ? (text.length > 0 ? 76 : 32) : (text.length > 0 ? 110 : 40)
    icon.width: 16
    icon.height: 16
    readonly property string iconSourceText: String(control.icon.source)

    background: Rectangle {
        radius: control.rounded ? height / 2 : 11
        border.width: 1
        border.color: control.checked || control.down ? control.accentColor : control.borderColor
        color: !control.enabled
            ? "#10141a"
            : control.down
                ? control.pressedColor
                : control.checked
                    ? control.checkedSurfaceColor
                    : control.hovered
                        ? control.hoverColor
                        : control.surfaceColor
        antialiasing: true
    }

    contentItem: Item {
        implicitWidth: contentRow.implicitWidth
        implicitHeight: contentRow.implicitHeight

        RowLayout {
            id: contentRow
            anchors.centerIn: parent
            spacing: control.text.length > 0 && control.iconSourceText.length > 0 ? 8 : 0

            Image {
                visible: control.iconSourceText.length > 0
                source: control.iconSourceText
                fillMode: Image.PreserveAspectFit
                sourceSize.width: 16
                sourceSize.height: 16
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                opacity: control.enabled ? 1.0 : 0.55
            }

            Label {
                text: control.text
                color: control.checked ? control.checkedTextColor : control.textColor
                font.pixelSize: control.compact ? 12 : 13
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
