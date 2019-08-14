cmake_minimum_required(VERSION 3.1)
project(optimization_lib)

set(SOURCES
	src/solvers/solver.cpp
	src/solvers/newton_solver.cpp
	src/solvers/gradient_descent_solver.cpp
	src/solvers/eigen_sparse_solver.cpp
	src/solvers/pardiso_solver.cpp


	src/objective_functions/objective_function.cpp
	src/objective_functions/objective_area_preserving.cpp
	src/objective_functions/total_objective.cpp
	src/objective_functions/objective_symmetric_dirichlet.cpp
	src/objective_functions/distortion_symmetric_dirichlet.cpp
	src/objective_functions/penalty_dynamics.cpp
	src/objective_functions/penalty_positional_constraints.cpp
	src/objective_functions/regularization_acceleration_l2.cpp
	src/objective_functions/constraints_positional.cpp


	include/solvers/solver.h
	include/solvers/newton_solver.h
	include/solvers/gradient_descent_solver.h
	include/solvers/eigen_sparse_solver.h
	include/solvers/pardiso_solver.h


	include/objective_functions/objective_function.h
	include/objective_functions/objective_area_preserving.h
	include/objective_functions/total_objective.h
	include/objective_functions/objective_symmetric_dirichlet.h
	include/objective_functions/distortion_symmetric_dirichlet.h
	include/objective_functions/penalty_dynamics.h
	include/objective_functions/penalty_positional_constraints.h
	include/objective_functions/regularization_acceleration_l2.h
	include/objective_functions/constraints_positional.h

	
	include/utils.h)


add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)