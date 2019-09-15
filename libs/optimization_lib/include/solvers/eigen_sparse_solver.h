#pragma once
#ifndef OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER
#define OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include <solvers/solver.h>

class EigenSparseSolver : public Solver
{
public:
	EigenSparseSolver();
	~EigenSparseSolver();
	void Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);

private:
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver_;
};

#endif