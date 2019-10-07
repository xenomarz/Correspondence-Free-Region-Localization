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

	virtual void AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A) = 0;
	virtual void AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A) = 0;

	virtual void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
	virtual void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
};

#endif