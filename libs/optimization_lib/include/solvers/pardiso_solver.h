#pragma once
#ifndef OPTIMIZATION_LIB_PARDISO_SOLVER
#define OPTIMIZATION_LIB_PARDISO_SOLVER

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./solver.h"

class PardisoSolver : public Solver
{
public:
	PardisoSolver();
	~PardisoSolver();
	void Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);
};

#endif