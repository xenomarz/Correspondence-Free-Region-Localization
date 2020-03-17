#pragma once

#include "libs/optimization_lib/include/solvers/solver.h"
#include "libs/optimization_lib/include/solvers/EigenSparseSolver.h"

class NewtonSolver : public solver
{
public:
	NewtonSolver(const int solverID): solver(solverID) {}
	virtual void step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
	
	Eigen::SparseMatrix<double> get_Hessian();
	double get_MSE();
private:
	std::unique_ptr<EigenSparseSolver<std::vector<int>, std::vector<double>>> eigen_solver = nullptr;
};