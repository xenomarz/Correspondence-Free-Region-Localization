cmake_minimum_required(VERSION 3.1)
project(optimization_lib)

set(SOURCES
	src/objective_functions/objective_function.cpp
	src/objective_functions/distortion_symmetric_dirichlet.cpp
	src/objective_functions/objective_symmetric_dirichlet.cpp
	include/objective_functions/objective_function.h
	include/objective_functions/distortion_symmetric_dirichlet.h
	include/objective_functions/objective_symmetric_dirichlet.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)		