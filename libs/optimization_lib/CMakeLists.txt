cmake_minimum_required(VERSION 3.15)
project(optimization_lib)

# Sources
file(GLOB SOURCES
	src/objective_functions/objective_function.cpp
	src/objective_functions/concrete_objective.cpp
	src/objective_functions/composite_objective.cpp
	src/objective_functions/vertex_position_objective.cpp
	src/objective_functions/barycenter_position_objective.cpp
	src/objective_functions/symmetric_dirichlet_objective.cpp
	src/objective_functions/separation_objective.cpp
	src/objective_functions/position_objective.cpp
	src/iterative_methods/iterative_method.cpp
	src/iterative_methods/newton_method.cpp
	src/iterative_methods/gradient_descent.cpp
	src/solvers/solver.cpp
	src/solvers/eigen_sparse_solver.cpp
	src/solvers/pardiso_solver.cpp
	src/utils/utils.cpp
	src/utils/objective_function_data_provider.cpp
	src/utils/mesh_wrapper.cpp
	include/objective_functions/objective_function.h
	include/objective_functions/concrete_objective.h
	include/objective_functions/composite_objective.h
	include/objective_functions/vertex_position_objective.h
	include/objective_functions/barycenter_position_objective.h
	include/objective_functions/symmetric_dirichlet_objective.h
	include/objective_functions/separation_objective.h
	include/objective_functions/position_objective.h
	include/iterative_methods/iterative_method.h
	include/iterative_methods/newton_method.h
	include/iterative_methods/gradient_descent.h
	include/solvers/solver.h	
	include/solvers/eigen_sparse_solver.h
	include/solvers/pardiso_solver.h
	include/utils/utils.h
	include/utils/objective_function_data_provider.h
	include/utils/mesh_wrapper.h)

# Add Library Target
add_library(${PROJECT_NAME} ${SOURCES})
add_library(rds::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Include Directories
target_include_directories(${PROJECT_NAME}
	PRIVATE
		${Boost_INCLUDE_DIRS}
		${PROJECT_SOURCE_DIR}/include)

# Link Libraries
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		igl::core)

find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
		target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	VS_GLOBAL_UseIntelMKL "Parallel"
	VS_GLOBAL_UseIntelTBB "Yes")

if (MSVC)
	# Turn on the __cplusplus flag in MSVC, so the __cplusplus macro will report the correct C++ version
	# https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=vs-2019
	# http://eigen.tuxfamily.org/bz/show_bug.cgi?id=1309
	target_compile_options(${PROJECT_NAME} PRIVATE /Zc:__cplusplus)
endif()

# Source Tree
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES})