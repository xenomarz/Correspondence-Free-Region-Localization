#pragma once
#ifndef OPTIMIZATION_LIB_SOLVER_H
#define OPTIMIZATION_LIB_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class Solver
{
public:
	Solver();
	~Solver();
	virtual void Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
	virtual void Solve(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
};

#endif