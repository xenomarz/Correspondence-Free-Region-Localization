#pragma once

#include <libs/optimization_lib/include/solvers/solver.h>
#include <iostream>
#include <Eigen/SparseCholesky>

using namespace std;

class GradientDescent : public Solver
{
public:
	GradientDescent();

	double step();
	bool test_progress();
	void internal_init();

private:
	MatrixXd X_new;
	// norm of the progress on the mesh
	double diff_norm;
	long long int prevTime;
};