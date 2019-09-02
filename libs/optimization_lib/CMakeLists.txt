cmake_minimum_required(VERSION 3.11)
project(optimization_lib)

file(GLOB SOURCES
	src/objective_functions/objective_function.cpp
	src/objective_functions/symmetric_dirichlet.cpp
	src/objective_functions/separation.cpp
	src/objective_functions/total_objective.cpp
	src/solvers/newton.cpp
	src/solvers/eigen_sparse_solver.cpp
	src/solvers/pardiso_solver.cpp
	src/solvers/solver.cpp
	src/utils/utils.cpp
	src/utils/mesh_wrapper.cpp
	include/objective_functions/objective_function.h
	include/objective_functions/symmetric_dirichlet.h
	include/objective_functions/separation.h
	include/objective_functions/total_objective.h
	include/solvers/newton.h
	include/solvers/eigen_sparse_solver.h
	include/solvers/pardiso_solver.h
	include/solvers/solver.h
	include/utils/utils.h
	include/utils/mesh_wrapper.h)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES})