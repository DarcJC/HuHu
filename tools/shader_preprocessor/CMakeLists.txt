
file(GLOB SOURCES_FILES src/main.cpp)

add_executable(ShaderPreprocessor ${SOURCES_FILES})

target_include_directories(ShaderPreprocessor PRIVATE include)
target_link_libraries(ShaderPreprocessor Boost::wave)
