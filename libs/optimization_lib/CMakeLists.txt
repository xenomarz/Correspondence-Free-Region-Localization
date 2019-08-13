cmake_minimum_required(VERSION 3.1)
project(optimization_lib)

set(SOURCES
	src/solvers/solver.cpp
	src/solvers/newton_solver.cpp
	src/solvers/gradient_descent_solver.cpp
	src/objective_functions/total_energy_function.cpp
	include/solvers/solver.h
	include/solvers/newton_solver.h
	include/solvers/gradient_descent_solver.h
	include/objective_functions/objective_function.h
	include/objective_functions/spring.h
	include/objective_functions/area_preserving.h
	include/objective_functions/total_energy_function.h
	include/objective_functions/rosenbrock_function.h
	include/objective_functions/element.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)