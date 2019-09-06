#pragma once
//#define USE_PARDISO

#include <libs/optimization_lib/include/solvers/solver.h>

#ifdef USE_PARDISO
	#include <libs/optimization_lib/include/solvers/pardiso_solver.h>
#else
	#include <libs/optimization_lib/include/solvers/eigen_sparse_solver.h>
#endif

class Newton : public Solver
{
public:
	Newton() {}
	virtual double step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;

private:
	#ifdef USE_PARDISO
		unique_ptr<PardisoSolver<vector<int>, vector<double>>> pardiso = nullptr;
	#else
		unique_ptr<EigenSparseSolver<vector<int>, vector<double>>> eigen_solver = nullptr;
	#endif
};