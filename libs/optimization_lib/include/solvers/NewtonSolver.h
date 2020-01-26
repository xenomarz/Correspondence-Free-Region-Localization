#pragma once
//#define USE_PARDISO

#include "libs/optimization_lib/include/solvers/solver.h"

#ifdef USE_PARDISO
	#include "libs/optimization_lib/include/solvers/PardisoSolver.h"
#else
	#include "libs/optimization_lib/include/solvers/EigenSparseSolver.h"
#endif

class NewtonSolver : public solver
{
public:
	NewtonSolver(const bool isConstrObjFunc, const int solverID): solver(isConstrObjFunc,solverID) {}
	virtual double step() override;
	virtual double aug_step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
	virtual void internal_aug_init() override;

	Eigen::SparseMatrix<double> get_Hessian();
	double get_MSE();
private:
	#ifdef USE_PARDISO
		std::unique_ptr<PardisoSolver<std::vector<int>, std::vector<double>>> pardiso = nullptr;
	#else
		std::unique_ptr<EigenSparseSolver<std::vector<int>, std::vector<double>>> eigen_solver = nullptr;
	#endif
};