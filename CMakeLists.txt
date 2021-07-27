cmake_minimum_required(VERSION 3.0.0)

# prject name
project(Points)

add_executable(Points)

target_sources(Points PRIVATE
    src/angle.h
    src/arcball_imgui.h
    src/arcball_imgui.cpp
    src/camera.h
    src/camera.cpp
    src/common.h
    src/dimension.h
    src/main.cpp
    src/maths.h
    src/shader.h
    src/shaders.cpp
    src/shapes.cpp
    src/shapes.h
    src/stb_image.h
    src/texture.h
    src/texture.cpp
    src/types.h
    src/utils.h
    src/vertex.h
    src/raytracing/raytrace.cpp
)

set_property(TARGET Points PROPERTY CXX_STANDARD 20)

# find dependencies
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
# set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

# link dependencies
target_link_libraries(Points OpenGL::GL glfw imgui::imgui implot::implot glad::glad glm::glm)


# "make run" target
# add_custom_target(run
#     COMMAND ${BIN_NAME}
#     DEPENDS ${BIN_NAME}
#   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
