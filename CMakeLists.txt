cmake_minimum_required(VERSION 3.27)

project(VolchayVoicer
    VERSION 0.1.0
    DESCRIPTION "Qt6 QML real-time C++ voice changer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Quick Qml QuickControls2 Svg Test)
find_package(spdlog CONFIG REQUIRED)
find_package(PortAudio REQUIRED)
find_package(RubberBand REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

add_library(volchay_core STATIC
    src/app/AppInfo.h
    src/audio/AudioDeviceModel.cpp
    src/audio/AudioDeviceModel.h
    src/audio/AudioEngine.cpp
    src/audio/AudioEngine.h
    src/audio/AudioEngineController.cpp
    src/audio/AudioEngineController.h
    src/audio/AudioTypes.h
    src/audio/PortAudioRuntime.cpp
    src/audio/PortAudioRuntime.h
    src/config/AppSettings.cpp
    src/config/AppSettings.h
    src/dsp/AudioEffect.h
    src/dsp/BasicEffects.cpp
    src/dsp/BasicEffects.h
    src/dsp/EffectSettings.h
    src/dsp/EffectsChain.cpp
    src/dsp/EffectsChain.h
    src/logging/LogController.cpp
    src/logging/LogController.h
    src/logging/Logger.cpp
    src/logging/Logger.h
    src/presets/PresetModel.cpp
    src/presets/PresetModel.h
)

target_include_directories(volchay_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_definitions(volchay_core PUBLIC
    VOLCHAY_VERSION="${PROJECT_VERSION}"
)
target_link_libraries(volchay_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        PortAudio::PortAudio
        RubberBand::RubberBand
        spdlog::spdlog
)

qt_add_executable(VolchayVoicer
    WIN32
    src/main.cpp
)

target_link_libraries(VolchayVoicer
    PRIVATE
        volchay_core
        Qt6::Quick
        Qt6::Qml
        Qt6::QuickControls2
        Qt6::Svg
)

qt_add_qml_module(VolchayVoicer
    URI VolchayVoicer
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ui/DarkButton.qml
        qml/ui/DarkProgressBar.qml
        qml/ui/DarkSlider.qml
        qml/ui/DarkTabButton.qml
    RESOURCES
        assets/icons/activity.svg
        assets/icons/logs.svg
        assets/icons/mic.svg
        assets/icons/power.svg
        assets/icons/preset.svg
        assets/icons/swap.svg
        assets/icons/warning.svg
        assets/icons/wave.svg
)

include(CTest)
if(BUILD_TESTING)
    add_executable(volchay_tests
        tests/dsp_tests.cpp
        tests/preset_tests.cpp
    )
    target_link_libraries(volchay_tests
        PRIVATE
            volchay_core
            Qt6::Test
    )

    get_filename_component(QT_PREFIX_DIR "${Qt6_DIR}/../../../" ABSOLUTE)
    add_test(NAME volchay_tests COMMAND $<TARGET_FILE:volchay_tests>)
    set_tests_properties(volchay_tests PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${QT_PREFIX_DIR}/bin;PATH=path_list_prepend:${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin"
    )
endif()
