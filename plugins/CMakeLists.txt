cmake_minimum_required(VERSION 3.1)
project(plugins)

set(SOURCES
	basic_menu/src/basic_menu.cpp
	basic_menu/include/basic_menu.h
	basic_menu/include/menu_utils.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_SOURCE_DIR}
		${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core
		igl::opengl
		igl::opengl_glfw
		igl::opengl_glfw_imgui
		rds::optimization_lib)		