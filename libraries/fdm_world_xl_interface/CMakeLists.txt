project(fdm_world_xl_interface)

set(includes includes/)
set(sources src/xlExport.cpp)

add_library(${PROJECT_NAME} SHARED ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${includes} ${common_includes_dir})