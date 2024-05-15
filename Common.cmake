# Common.cmake

function(configure_target TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIR})
    target_sources(${TARGET_NAME} PRIVATE ${SOURCES} ${HEADERS})

	# Headers
    foreach(_header IN ITEMS ${HEADERS})
        get_filename_component(_header_path "${_header}" PATH)
        string(REPLACE "${CMAKE_SOURCE_DIR}/${TARGET_NAME}" "" _header_path "${_header_path}")
        string(REPLACE "/" "\\" _header_path "${_header_path}")
        source_group("${_header_path}" FILES "${_header}")
    endforeach()

	# Sources
    foreach(_source IN ITEMS ${SOURCES})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "${CMAKE_SOURCE_DIR}/${TARGET_NAME}" "" _group_path "${_source_path}")
        string(REPLACE "/" "\\" _group_path "${_group_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endfunction()