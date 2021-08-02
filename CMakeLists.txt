cmake_minimum_required(VERSION 3.0.0)

# prject name
project(oneWeekRt)

add_executable(oneWeekRt)

target_sources(oneWeekRt PRIVATE
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
    src/raytracing/aabb.h
    src/raytracing/aarect.h
    src/raytracing/box.h
    src/raytracing/bvh.h
    src/raytracing/camera.h
    src/raytracing/hittable.h
    src/raytracing/hittable_list.h
    src/raytracing/image.h
    src/raytracing/material.h
    src/raytracing/onb.h
    src/raytracing/pdf.h
    src/raytracing/perlin.h
    src/raytracing/ray.h
    src/raytracing/raytrace.h
    src/raytracing/rtweekend.h
    src/raytracing/sphere.h
    src/raytracing/stopwatch.h
    src/raytracing/texture.h
    src/raytracing/vec3.h
)

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX NOGDI)

set_property(TARGET oneWeekRt PROPERTY CXX_STANDARD 20)
# find dependencies
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
# set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
target_compile_options(oneWeekRt PRIVATE /arch:AVX2)

# link dependencies
target_link_libraries(oneWeekRt OpenGL::GL glfw imgui::imgui implot::implot glad::glad glm)
