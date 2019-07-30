cmake_minimum_required(VERSION 3.1)
project(RDS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# libigl
option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   ON)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     ON)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    ON)
find_package(LIBIGL REQUIRED QUIET)

# libraries and plugins
add_subdirectory("libs/optimization-lib")
add_subdirectory("plugins")

if (CMAKE_JS_VERSION)
    add_subdirectory("node-addon")
endif()

# applications
add_subdirectory("apps/autoquads")