import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import VolchayVoicer

ApplicationWindow {
    id: root
    width: 1280
    height: 820
    visible: true
    title: "Volchay Voicer"
    color: appBackground
    property int selectedPresetIndex: 0
    property string activeCompareSlot: "A"
    property var compareSlotA: ({})
    property var compareSlotB: ({})
    property bool compareReady: false
    property color appBackground: "#0b0f14"
    property color panelBackground: "#141b24"
    property color panelBackgroundAlt: "#10161d"
    property color panelBorder: "#243042"
    property color accentColor: "#2fd27f"
    property color accentBlue: "#5a8dff"
    property color accentCyan: "#57d0ff"
    property color accentWarning: "#f0b35f"

    function formatDb(value) {
        return (Math.round(value * 10) / 10).toFixed(1) + " dB"
    }

    function formatPercent(value) {
        return Math.round(value * 100) + " %"
    }

    function formatNumber(value) {
        return Math.round(value * 10) / 10
    }

    function cloneSettings(map) {
        return JSON.parse(JSON.stringify(map || {}))
    }

    function isEmptyMap(map) {
        return !map || Object.keys(map).length === 0
    }

    function syncCompareSlotFromLive() {
        if (!compareReady) {
            return
        }
        const snapshot = cloneSettings(audioController.effectSettings)
        if (activeCompareSlot === "A") {
            compareSlotA = snapshot
        } else {
            compareSlotB = snapshot
        }
    }

    function loadCompareSlot(slot) {
        const snapshot = slot === "A" ? compareSlotA : compareSlotB
        if (isEmptyMap(snapshot)) {
            return
        }
        audioController.applyPreset(snapshot)
    }

    function activateCompareSlot(slot) {
        if (!compareReady) {
            activeCompareSlot = slot
            return
        }
        if (slot === activeCompareSlot) {
            loadCompareSlot(slot)
            return
        }
        syncCompareSlotFromLive()
        activeCompareSlot = slot
        loadCompareSlot(slot)
    }

    function swapCompareSlots() {
        if (!compareReady) {
            return
        }
        syncCompareSlotFromLive()
        const oldA = compareSlotA
        compareSlotA = compareSlotB
        compareSlotB = oldA
        activeCompareSlot = activeCompareSlot === "A" ? "B" : "A"
        loadCompareSlot(activeCompareSlot)
    }

    function applyPresetToActiveSlot(settings, index) {
        selectedPresetIndex = index
        audioController.applyPreset(settings)
    }

    function syncDeviceIndexes() {
        const inputIndex = audioController.selectedInputDevice
        const outputIndex = audioController.selectedOutputDevice
        let matchedInput = false
        for (let i = 0; i < inputDevices.count; ++i) {
            if (inputDevices.deviceIndexAt(i) === inputIndex) {
                inputCombo.currentIndex = i
                matchedInput = true
                break
            }
        }
        if (!matchedInput && inputDevices.defaultModelIndex >= 0) {
            inputCombo.currentIndex = inputDevices.defaultModelIndex
            audioController.selectedInputDevice = inputDevices.deviceIndexAt(inputDevices.defaultModelIndex)
        }
        let matchedOutput = false
        for (let i = 0; i < outputDevices.count; ++i) {
            if (outputDevices.deviceIndexAt(i) === outputIndex) {
                outputCombo.currentIndex = i
                matchedOutput = true
                break
            }
        }
        if (!matchedOutput && outputDevices.defaultModelIndex >= 0) {
            outputCombo.currentIndex = outputDevices.defaultModelIndex
            audioController.selectedOutputDevice = outputDevices.deviceIndexAt(outputDevices.defaultModelIndex)
        }
    }

    onVisibleChanged: if (visible) syncDeviceIndexes()

    Connections {
        target: audioController
        function onSettingsChanged() {
            if (compareReady) {
                syncCompareSlotFromLive()
            }
            syncDeviceIndexes()
        }
    }

    Connections {
        target: inputDevices
        function onDevicesChanged() {
            syncDeviceIndexes()
        }
    }

    Connections {
        target: outputDevices
        function onDevicesChanged() {
            syncDeviceIndexes()
        }
    }

    Timer {
        interval: 900
        running: true
        repeat: true
        onTriggered: logController.refresh()
    }

    header: ToolBar {
        height: 76
        background: Rectangle {
            color: root.panelBackgroundAlt
            border.color: root.panelBorder
        }
        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Image {
                source: "qrc:/qt/qml/VolchayVoicer/assets/icons/wave.svg"
                sourceSize.width: 28
                sourceSize.height: 28
                Layout.preferredWidth: 28
                Layout.preferredHeight: 28
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Label {
                    text: "Volchay Voicer"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#e8edf2"
                }
                Label {
                    text: "Windows x64 real-time voice changer"
                    color: "#93a4b4"
                }
            }

            Rectangle {
                radius: 8
                color: audioController.running ? "#123726" : "#2b1820"
                border.color: audioController.running ? root.accentColor : "#b84c59"
                implicitWidth: 116
                implicitHeight: 36
                Layout.alignment: Qt.AlignVCenter
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8
                    Image {
                        source: audioController.running ? "qrc:/qt/qml/VolchayVoicer/assets/icons/power.svg" : "qrc:/qt/qml/VolchayVoicer/assets/icons/warning.svg"
                        sourceSize.width: 16
                        sourceSize.height: 16
                        Layout.preferredWidth: 16
                        Layout.preferredHeight: 16
                    }
                    Label {
                        text: audioController.running ? "Live" : "Stopped"
                        color: "#ecf0f4"
                        font.pixelSize: 13
                    }
                }
            }

            DarkButton {
                text: audioController.running ? "Stop" : "Start"
                icon.source: audioController.running ? "qrc:/qt/qml/VolchayVoicer/assets/icons/power.svg" : "qrc:/qt/qml/VolchayVoicer/assets/icons/power.svg"
                accentColor: audioController.running ? "#d15a68" : root.accentColor
                surfaceColor: "#151d27"
                hoverColor: "#1b2430"
                pressedColor: "#0f141b"
                borderColor: "#2a3442"
                onClicked: audioController.toggle()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 14

            Rectangle {
                Layout.preferredWidth: 330
                Layout.fillHeight: true
                radius: 12
                color: root.panelBackground
                border.color: root.panelBorder

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Image {
                            source: "qrc:/qt/qml/VolchayVoicer/assets/icons/preset.svg"
                            sourceSize.width: 18
                            sourceSize.height: 18
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                        }
                        Label {
                            text: "Presets"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#e4ebf2"
                        }
                        Item { Layout.fillWidth: true }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Label {
                            text: "Compare"
                            color: "#8fa2b7"
                            font.pixelSize: 12
                        }

                        Item { Layout.fillWidth: true }

                        DarkButton {
                            text: "A"
                            compact: true
                            checkable: true
                            checked: root.activeCompareSlot === "A"
                            accentColor: root.accentBlue
                            surfaceColor: "#17202b"
                            hoverColor: "#1b2533"
                            pressedColor: "#0f141b"
                            borderColor: "#283548"
                            onClicked: root.activateCompareSlot("A")
                        }

                        DarkButton {
                            text: "B"
                            compact: true
                            checkable: true
                            checked: root.activeCompareSlot === "B"
                            accentColor: root.accentCyan
                            surfaceColor: "#17202b"
                            hoverColor: "#1b2533"
                            pressedColor: "#0f141b"
                            borderColor: "#283548"
                            onClicked: root.activateCompareSlot("B")
                        }

                        DarkButton {
                            text: "Swap"
                            compact: true
                            icon.source: "qrc:/qt/qml/VolchayVoicer/assets/icons/swap.svg"
                            accentColor: root.accentWarning
                            surfaceColor: "#17202b"
                            hoverColor: "#1b2533"
                            pressedColor: "#0f141b"
                            borderColor: "#283548"
                            onClicked: root.swapCompareSlots()
                        }
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 8
                        model: presetModel
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 72
                            radius: 10
                            color: index === root.selectedPresetIndex ? "#1d2532" : "#151b24"
                            border.color: index === root.selectedPresetIndex ? root.accentBlue : "#283344"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    root.applyPresetToActiveSlot(settings, index)
                                }
                            }

                            Column {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 4
                                Label {
                                    text: name
                                    font.pixelSize: 15
                                    font.bold: true
                                    color: "#eff3f7"
                                }
                                Label {
                                    width: parent.width
                                    wrapMode: Text.WordWrap
                                    text: description
                                    color: "#98a8b8"
                                    font.pixelSize: 12
                                }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 14

                Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 220
                radius: 12
                color: root.panelBackground
                border.color: root.panelBorder

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            Image {
                                source: "qrc:/qt/qml/VolchayVoicer/assets/icons/mic.svg"
                                sourceSize.width: 18
                                sourceSize.height: 18
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                            }
                            Label {
                                text: "Audio devices"
                                font.pixelSize: 18
                                font.bold: true
                                color: "#e4ebf2"
                            }
                            Item { Layout.fillWidth: true }
                            DarkButton {
                                text: "Refresh"
                                compact: true
                                accentColor: root.accentBlue
                                surfaceColor: "#17202b"
                                hoverColor: "#1b2533"
                                pressedColor: "#0f141b"
                                borderColor: "#283548"
                                enabled: !audioController.running
                                onClicked: {
                                    inputDevices.refresh()
                                    outputDevices.refresh()
                                    syncDeviceIndexes()
                                }
                            }
                            Label {
                                text: audioController.errorMessage
                                color: "#d68b4f"
                                visible: audioController.errorMessage.length > 0
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            Label { text: "Input"; color: "#a9b7c4"; Layout.preferredWidth: 52 }
                            ComboBox {
                                id: inputCombo
                                Layout.fillWidth: true
                                model: inputDevices
                                textRole: "displayName"
                                valueRole: "deviceIndex"
                                enabled: !audioController.running
                                onActivated: audioController.selectedInputDevice = currentValue
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            Label { text: "Output"; color: "#a9b7c4"; Layout.preferredWidth: 52 }
                            ComboBox {
                                id: outputCombo
                                Layout.fillWidth: true
                                model: outputDevices
                                textRole: "displayName"
                                valueRole: "deviceIndex"
                                enabled: !audioController.running
                                onActivated: audioController.selectedOutputDevice = currentValue
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            Label { text: "Sample"; color: "#a9b7c4"; Layout.preferredWidth: 52 }
                            SpinBox {
                                from: 22050
                                to: 96000
                                stepSize: 1000
                                value: audioController.sampleRate
                                enabled: !audioController.running
                                onValueModified: audioController.sampleRate = value
                            }
                            Label { text: "Buffer"; color: "#a9b7c4"; Layout.preferredWidth: 50 }
                            SpinBox {
                                from: 64
                                to: 2048
                                stepSize: 64
                                value: audioController.framesPerBuffer
                                enabled: !audioController.running
                                onValueModified: audioController.framesPerBuffer = value
                            }
                            Item { Layout.fillWidth: true }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 12
                    color: root.panelBackground
                    border.color: root.panelBorder

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            Image {
                                source: "qrc:/qt/qml/VolchayVoicer/assets/icons/activity.svg"
                                sourceSize.width: 18
                                sourceSize.height: 18
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                            }
                            Label {
                                text: "Voice controls"
                                font.pixelSize: 18
                                font.bold: true
                                color: "#e4ebf2"
                            }
                            Item { Layout.fillWidth: true }
                            Label {
                                text: "Latency " + Math.round(audioController.latencyMs) + " ms"
                                color: "#9cb3c7"
                            }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            columns: 2
                            columnSpacing: 16
                            rowSpacing: 10
                            Layout.fillHeight: true

                            Repeater {
                                model: [
                                    { key: "pitchSemitones", label: "Pitch", from: -12, to: 12, step: 0.5, defaultValue: 0 },
                                    { key: "formantSemitones", label: "Formant", from: -12, to: 12, step: 0.5, defaultValue: 0 },
                                    { key: "robotMix", label: "Robot", from: 0, to: 1, step: 0.01, defaultValue: 0 },
                                    { key: "radioMix", label: "Radio", from: 0, to: 1, step: 0.01, defaultValue: 0 },
                                    { key: "echoMix", label: "Echo", from: 0, to: 1, step: 0.01, defaultValue: 0 },
                                    { key: "outputGainDb", label: "Gain", from: -12, to: 12, step: 0.5, defaultValue: 0 },
                                    { key: "noiseGateThresholdDb", label: "Gate", from: -80, to: -10, step: 1, defaultValue: -58 },
                                    { key: "compressorThresholdDb", label: "Comp", from: -30, to: 0, step: 1, defaultValue: -12 }
                                ]

                                delegate: DarkSlider {
                                    Layout.fillWidth: true
                                    label: modelData.label
                                    valueText: modelData.key === "robotMix" || modelData.key === "radioMix" || modelData.key === "echoMix"
                                        ? formatPercent(value)
                                        : formatNumber(value)
                                    from: modelData.from
                                    to: modelData.to
                                    stepSize: modelData.step
                                    value: audioController.effectSettings[modelData.key] !== undefined
                                        ? audioController.effectSettings[modelData.key]
                                        : modelData.defaultValue
                                    accentColor: modelData.key === "pitchSemitones" || modelData.key === "formantSemitones"
                                        ? root.accentBlue
                                        : root.accentColor
                                    onMoved: audioController.setEffectParameter(modelData.key, value)
                                }
                            }
                        }
                    }
                }
            }
        }

        TabBar {
            id: tabs
            Layout.fillWidth: true
            background: Rectangle {
                color: "transparent"
            }
            DarkTabButton { text: "Diagnostics"; width: tabs.width / 2 }
            DarkTabButton { text: "Logs"; width: tabs.width / 2 }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            currentIndex: tabs.currentIndex

            Rectangle {
                radius: 12
                color: root.panelBackground
                border.color: root.panelBorder

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 18

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Label { text: "Input"; color: "#93a4b4" }
                        DarkProgressBar {
                            value: audioController.inputLevel
                            from: 0
                            to: 1
                            Layout.fillWidth: true
                            accentColor: root.accentBlue
                        }
                        Label { text: "Output"; color: "#93a4b4" }
                        DarkProgressBar {
                            value: audioController.outputLevel
                            from: 0
                            to: 1
                            Layout.fillWidth: true
                            accentColor: root.accentColor
                        }
                    }

                    ColumnLayout {
                        Layout.preferredWidth: 220
                        spacing: 6
                        Label { text: "CPU"; color: "#93a4b4" }
                        DarkProgressBar {
                            value: audioController.cpuLoad
                            from: 0
                            to: 1
                            Layout.fillWidth: true
                            accentColor: root.accentWarning
                        }
                        Label {
                            text: "Running: " + (audioController.running ? "yes" : "no")
                            color: "#d9e3eb"
                        }
                        Label {
                            text: "Preset slot is ready for AI later"
                            color: "#7e91a4"
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            Rectangle {
                radius: 12
                color: root.panelBackground
                border.color: root.panelBorder

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Image {
                            source: "qrc:/qt/qml/VolchayVoicer/assets/icons/logs.svg"
                            sourceSize.width: 18
                            sourceSize.height: 18
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                        }
                        Label {
                            text: "Log file: " + logController.logPath
                            color: "#9cb3c7"
                            elide: Text.ElideMiddle
                            Layout.fillWidth: true
                        }
                    }

                    TextArea {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: logController.lines.join("\n")
                        readOnly: true
                        wrapMode: TextEdit.NoWrap
                        color: "#dfe6ee"
                        background: Rectangle {
                            color: root.panelBackgroundAlt
                            radius: 8
                            border.width: 1
                            border.color: root.panelBorder
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        const snapshot = cloneSettings(audioController.effectSettings)
        compareSlotA = snapshot
        compareSlotB = cloneSettings(snapshot)
        compareReady = true
        syncDeviceIndexes()
    }
}
