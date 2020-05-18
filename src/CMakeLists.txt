
cmake_minimum_required (VERSION 3.8)

add_executable (${PROJECT_NAME} main.cpp Shader.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Basic.fs.glsl
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Basic.vs.glsl
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} glfw glad)