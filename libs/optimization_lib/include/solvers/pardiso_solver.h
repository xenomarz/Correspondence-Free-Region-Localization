#pragma once
#ifndef OPTIMIZATION_LIB_PARDISO_SOLVER_H
#define OPTIMIZATION_LIB_PARDISO_SOLVER_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// MKL includes
#include "mkl_types.h"

// Optimization lib includes
#include "./solver.h"

// https://software.intel.com/en-us/mkl-developer-reference-c-intel-mkl-pardiso-parallel-direct-sparse-solver-interface
class PardisoSolver : public Solver
{
public:
	/**
	 * Constructors and destructor
	 */
	PardisoSolver();
	~PardisoSolver();

	/**
	 * Public overrides
	 */
	void AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A);
	void AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A);
	void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);
	void Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x);

private:
	/**
	 * Fields
	 */
	MKL_INT n_;
	std::unique_ptr<MKL_INT[]> ia_;
	std::unique_ptr<MKL_INT[]> ja_;
	MKL_INT mtype_;
	MKL_INT nrhs_;
	MKL_INT iparm_[64];
	MKL_INT maxfct_;
	MKL_INT mnum_;
	MKL_INT phase_;
	MKL_INT error_;
	MKL_INT msglvl_;
	MKL_INT idum_;
	double ddum_;
	std::unique_ptr<double[]> a_;
	void* pt_[64];
};

#endif