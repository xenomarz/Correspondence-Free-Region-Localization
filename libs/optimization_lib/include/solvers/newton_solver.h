#pragma once
#define USE_PARDISO


#include <solvers/solver.h>

#ifdef USE_PARDISO
	#include <solvers/pardiso_solver.h>
#else
	#include <solvers/eigen_sparse_solver.h>
#endif

#include <iostream>
#include <Eigen/SparseCholesky>

using namespace std;

class Newton : public Solver
{
public:
	Newton();

	double step();
	bool test_progress();
	void internal_init();

private:
	// norm of the progress on the mesh
	double diff_norm;
	#ifdef USE_PARDISO
		unique_ptr<PardisoSolver<vector<int>, vector<double>>> pardiso = nullptr;
	#else
		unique_ptr<EigenSparseSolver<vector<int>, vector<double>>> eigen_solver = nullptr;
	#endif
	long long int prevTime;
};