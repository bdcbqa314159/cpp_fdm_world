project(app)

set(includes ./ 
            ../libraries/ 
            ../libraries/common/ 
            ../libraries/fdm_world_lib/ 
            ${dlloader_include_dir} 
            ${dlloader_include_dir_platform}
)

set(project1 fdm_test)

add_executable(${project1} ${project1}.cpp)
target_include_directories(${project1} PUBLIC ${includes})
target_link_libraries(${project1} fdm_world)