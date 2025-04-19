project(libraries)

set(common_includes_dir ${CMAKE_CURRENT_SOURCE_DIR}/common/)

add_subdirectory(fdm_world_lib)

if(WIN32)
    message(STATUS "Windows detected - xll interfaces")
    add_subdirectory(excel_sdk_api)
    add_subdirectory(fdm_world_xl_interface)
endif()
