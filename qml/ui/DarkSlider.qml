import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: control

    property string label: ""
    property string valueText: ""
    property color accentColor: "#2fd27f"
    property color trackColor: "#202835"
    property color handleColor: "#edf3f8"
    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize
    property alias value: slider.value
    signal moved(real value)

    implicitHeight: 58

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Label {
                text: control.label
                color: "#d9e3eb"
                font.pixelSize: 12
            }

            Item { Layout.fillWidth: true }

            Label {
                text: control.valueText
                color: "#8ca0b3"
                font.pixelSize: 12
            }
        }

        Slider {
            id: slider
            Layout.fillWidth: true
            hoverEnabled: true

            background: Item {
                implicitHeight: 18

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    height: 5
                    radius: 2.5
                    color: control.trackColor
                }

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: slider.visualPosition * parent.width
                    height: 5
                    radius: 2.5
                    color: control.accentColor
                }
            }

            handle: Rectangle {
                x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                width: 18
                height: 18
                radius: 9
                color: slider.pressed ? Qt.lighter(control.handleColor, 1.05) : control.handleColor
                border.width: 1
                border.color: control.accentColor
                antialiasing: true
            }

            onMoved: control.moved(value)
        }
    }
}
