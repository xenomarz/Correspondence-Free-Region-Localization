#pragma once
#ifndef OPTIMIZATION_LIB_SOLVER_H
#define OPTIMIZATION_LIB_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

template<Eigen::StorageOptions StorageOrder>
class Solver
{
public:
	Solver()
	{
		
	}
	
	virtual ~Solver()
	{
		
	}

	virtual void AnalyzePattern(const Eigen::SparseMatrix<double, StorageOrder>& A) = 0;
	virtual void Solve(const Eigen::SparseMatrix<double, StorageOrder>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x) = 0;
};

#endif