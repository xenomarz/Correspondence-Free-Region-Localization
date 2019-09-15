#pragma once
#ifndef OPTIMIZATION_LIB_SOLVER
#define OPTIMIZATION_LIB_SOLVER

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class Solver
{
public:
	Solver();
	~Solver();
	virtual void Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
};

#endif