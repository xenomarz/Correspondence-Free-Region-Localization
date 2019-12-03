cmake_minimum_required(VERSION 3.1)
project(RDS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# boost
set(BOOST_ROOT "./boost")
find_package(Boost REQUIRED QUIET)

#eigen
add_subdirectory("eigen" EXCLUDE_FROM_ALL)
set(Eigen3_DIR "${CMAKE_CURRENT_BINARY_DIR}/eigen")
find_package (Eigen3 3.3 REQUIRED)

# libigl
option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   ON)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     ON)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    ON)
find_package(LIBIGL REQUIRED QUIET)

# libraries and plugins
add_subdirectory("libs/optimization_lib")
add_subdirectory("plugins")

# gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("gtest" EXCLUDE_FROM_ALL)

# tests
add_subdirectory("tests/optimization_lib_tests")

# node addons
if (CMAKE_JS_VERSION)
    add_subdirectory("node_addon")
endif()

# applications
add_subdirectory("apps/autoquads")
add_subdirectory("apps/console_app")