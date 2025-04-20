project(libraries)

set(common_includes_dir ${CMAKE_CURRENT_SOURCE_DIR}/common/)

add_subdirectory(fdm_world_lib)
set(fdm_world_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/fdm_world_lib)
set(common_dir ${CMAKE_CURRENT_SOURCE_DIR}/common)

if(WIN32)
    message(STATUS "Windows detected - xll interfaces")
    set(excel_sdk_api_dir ${CMAKE_CURRENT_SOURCE_DIR}/excel_sdk_api)

    add_subdirectory(excel_sdk_api)
    add_subdirectory(fdm_world_xl_interface)
endif()
