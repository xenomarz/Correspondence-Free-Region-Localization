cmake_minimum_required(VERSION 3.1)
project(optimization-lib)

set(SOURCES
	src/objective-function.cpp
	src/distortion-symmetric-dirichlet.cpp
	src/objective-symmetric-dirichlet.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)		