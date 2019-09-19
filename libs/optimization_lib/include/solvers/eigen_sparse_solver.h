#pragma once
#ifndef OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER_H
#define OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./solver.h"

class EigenSparseSolver : public Solver
{
public:
	EigenSparseSolver();
	~EigenSparseSolver();
	void Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);
	void Solve(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, const Eigen::VectorXd& b, Eigen::VectorXd& x);

private:
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver_;
};

#endif