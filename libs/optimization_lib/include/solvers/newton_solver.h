#pragma once
//#define USE_PARDISO

#include <libs/optimization_lib/include/solvers/solver.h>


#ifdef USE_PARDISO
	#include <libs/optimization_lib/include/solvers/pardiso_solver.h>
#else
	#include <libs/optimization_lib/include/solvers/eigen_sparse_solver.h>
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

	void init_new(MatrixXd& X) {
		X_new = X;
	}

	void solve_new() {
		X_new = 1.0001*X_new;
	}

	void get_data_new(MatrixXd& X) {
		X = this->X_new;
	}

private:
	MatrixXd X_new;
	// norm of the progress on the mesh
	double diff_norm;
	#ifdef USE_PARDISO
		unique_ptr<PardisoSolver<vector<int>, vector<double>>> pardiso = nullptr;
	#else
		unique_ptr<EigenSparseSolver<vector<int>, vector<double>>> eigen_solver = nullptr;
	#endif
	long long int prevTime;
};