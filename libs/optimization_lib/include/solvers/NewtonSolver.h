#pragma once

#include "libs/optimization_lib/include/solvers/solver.h"
#include "libs/optimization_lib/include/solvers/EigenSparseSolver.h"

class NewtonSolver : public solver
{
public:
	NewtonSolver(const bool isConstrObjFunc, const int solverID): solver(isConstrObjFunc,solverID) {}
	virtual void step() override;
	virtual void aug_step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
	virtual void internal_aug_init() override;

	Eigen::SparseMatrix<double> get_Hessian();
	double get_MSE();
private:
	std::unique_ptr<EigenSparseSolver<std::vector<int>, std::vector<double>>> eigen_solver = nullptr;
};