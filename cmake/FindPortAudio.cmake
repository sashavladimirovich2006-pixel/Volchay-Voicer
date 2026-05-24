find_path(PORTAUDIO_INCLUDE_DIR
    NAMES portaudio.h
)

find_library(PORTAUDIO_LIBRARY
    NAMES portaudio portaudio_static portaudio_x64 portaudio_x64_static
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio
    REQUIRED_VARS PORTAUDIO_INCLUDE_DIR PORTAUDIO_LIBRARY
)

if(PortAudio_FOUND AND NOT TARGET PortAudio::PortAudio)
    add_library(PortAudio::PortAudio UNKNOWN IMPORTED)
    set_target_properties(PortAudio::PortAudio PROPERTIES
        IMPORTED_LOCATION "${PORTAUDIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PORTAUDIO_INCLUDE_DIR}"
    )
endif()
