cmake_minimum_required(VERSION 3.1)
project(plugins)

set(SOURCES
	basic-menu/src/basic-menu.cpp
	basic-menu/include/basic-menu.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core
		igl::opengl
		igl::opengl_glfw
		igl::opengl_glfw_imgui)		