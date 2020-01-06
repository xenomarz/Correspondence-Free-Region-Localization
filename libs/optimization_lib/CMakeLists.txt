cmake_minimum_required(VERSION 3.5)
# set the project name
project(optimization_lib)

file(GLOB sources
	src/solvers/*.cpp
	src/objective_functions/*.cpp
	include/solvers/*.h
	include/objective_functions/*.h
	include/utils.h
	)


add_library(${PROJECT_NAME} ${sources})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/include
		${CMAKE_SOURCE_DIR}/ext/worhp/worhp)

add_library(worhp SHARED IMPORTED)
set_property(TARGET worhp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/ext/worhp/bin/worhp.dll)
set_property(TARGET worhp PROPERTY IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/ext/worhp/bin/worhp.lib)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core
		worhp
		igl::matlab
		)