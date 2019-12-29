#pragma once

#include <libs/optimization_lib/include/objective_functions/TotalObjective.h>
#include <libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h>
#include <atomic>
#include <shared_mutex>
#include <igl/flip_avoiding_line_search.h>
#include <Eigen/SparseCholesky>

class worhpSolver
{
public:
	worhpSolver(const bool IsConstrObjFunc, const int solverID);
	~worhpSolver();
	int run();
	void run_one_iteration(const int steps);
};