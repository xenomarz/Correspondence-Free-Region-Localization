#pragma once
#ifndef OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER_H
#define OPTIMIZATION_LIB_EIGEN_SPARSE_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./solver.h"

class EigenSparseSolver : public Solver<Eigen::StorageOptions::ColMajor>
{
public:
	/**
	 * Constructors and destructor
	 */
	EigenSparseSolver()
	{
		
	}
	
	virtual ~EigenSparseSolver()
	{
		
	}

	/**
	 * Overrides
	 */
	void EigenSparseSolver::AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A) override
	{
		solver_.analyzePattern(A);
	}

	void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) override
	{
		// Compute the numerical factorization 
		solver_.factorize(A);

		// Use the factors to solve the linear system 
		x = solver_.solve(b);
	}

private:
	/**
	 * Private fields
	 */
	Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver_;
};

#endif