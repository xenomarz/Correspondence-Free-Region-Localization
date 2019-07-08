cmake_minimum_required(VERSION 3.1)
project(optimization-lib)

set(SOURCES
	src/objective-function.cpp
	src/distortion-symmetric-dirichlet.cpp
	src/objective-symmetric-dirichlet.cpp
	include/objective-function.h
	include/distortion-symmetric-dirichlet.h
	include/objective-symmetric-dirichlet.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)		