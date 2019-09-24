cmake_minimum_required(VERSION 3.5)
project(optimization_lib)

file(GLOB sources
	src/solvers/*.cpp
	src/objective_functions/*.cpp
	include/solvers/*.h
	include/objective_functions/*.h
	include/utils.h)


add_library(${PROJECT_NAME} ${sources})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)