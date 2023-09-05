file(GLOB_RECURSE CoreFileList *.cpp)

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
