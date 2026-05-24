find_path(RUBBERBAND_INCLUDE_DIR
    NAMES rubberband/RubberBandLiveShifter.h
)

find_library(RUBBERBAND_LIBRARY
    NAMES rubberband rubberband-2 rubberband-3 rubberband-4
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RubberBand
    REQUIRED_VARS RUBBERBAND_INCLUDE_DIR RUBBERBAND_LIBRARY
)

if(RubberBand_FOUND AND NOT TARGET RubberBand::RubberBand)
    add_library(RubberBand::RubberBand UNKNOWN IMPORTED)
    set_target_properties(RubberBand::RubberBand PROPERTIES
        IMPORTED_LOCATION "${RUBBERBAND_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${RUBBERBAND_INCLUDE_DIR}"
    )
endif()
