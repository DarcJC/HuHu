file(GLOB_RECURSE CoreFileList core/core.cpp core/mesh_collector.cpp core/gpu_memory.cpp)
file(GLOB_RECURSE DisplayFileList display/display.cpp)

add_library(Rendering STATIC ${CoreFileList} ${DisplayFileList})
target_include_directories(Rendering PUBLIC include/)

target_link_libraries(Rendering PUBLIC
    dawn_internal_config
    dawncpp
    dawn_proc
    dawn_common
    dawn_glfw
    dawn_native
    dawn_wire
    dawn_utils
)

target_compile_definitions(Rendering PRIVATE
    RENDERING_EXPORT=1
)

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(Rendering PUBLIC Eigen3::Eigen)
