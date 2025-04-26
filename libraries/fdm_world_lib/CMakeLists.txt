project(fdm_world)

set(includes includes/)
set(sources src/solver.cpp
			src/Bachelier.cpp
			src/Black.cpp)

add_library(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PUBLIC ${includes} ${common_includes_dir})