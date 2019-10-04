#pragma once
#ifndef OPTIMIZATION_LIB_PARDISO_SOLVER_H
#define OPTIMIZATION_LIB_PARDISO_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./solver.h"

// https://software.intel.com/en-us/mkl-developer-reference-c-intel-mkl-pardiso-parallel-direct-sparse-solver-interface
class PardisoSolver : public Solver
{
public:
	PardisoSolver();
	~PardisoSolver();
	void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);
	void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);
};

#endif