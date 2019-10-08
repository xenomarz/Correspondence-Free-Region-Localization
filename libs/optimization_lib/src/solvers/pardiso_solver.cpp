/**
 * Copyright 2004-2019 Intel Corporation.
 *
 * The following code is based on examples provided by Intel MKL package.
 * You may find the examples under "${IntelSWTools_root}\compilers_and_libraries\windows\mkl\examples\examples_core_c.zip".
 * For further reference and documentation, please see:
 * https://software.intel.com/en-us/mkl-developer-reference-c-intel-mkl-pardiso-parallel-direct-sparse-solver-interface
 */

// Optimization lib includes
#include <solvers/pardiso_solver.h>
#include <utils/utils.h>

// MKL includes
#include "mkl_pardiso.h"
#include "mkl.h"

PardisoSolver::PardisoSolver()
{
	mkl_cbwr_set(mkl_cbwr_get_auto_branch());

	// TODO: Add setters to control pardiso's properties
	mtype_ = 2;			/* Real and symmetric positive definite */
	nrhs_ = 1;			/* Number of right hand sides. */

	/* -------------------------------------*/
	/* .. Setup Pardiso control parameters. */
	/* -------------------------------------*/
	for (MKL_INT i = 0; i < 64; i++)
	{
		iparm_[i] = 0;
	}

	iparm_[0] = 1;		/* No solver default */
	iparm_[1] = 3;		/* The parallel (OpenMP) version of the nested dissection algorithm */
	iparm_[23] = 1;		/* Intel MKL PARDISO uses a two-level factorization algorithm */
	iparm_[24] = 1;		/* Intel MKL PARDISO uses a parallel algorithm for the solve step */
	iparm_[7] = 1;		/* Max numbers of iterative refinement steps */
	iparm_[34] = 1;		/* PARDISO use C-style indexing for ia and ja arrays */
	maxfct_ = 1;		/* Maximum number of numerical factorizations. */
	mnum_ = 1;			/* Which factorization to use. */
	msglvl_ = 0;		/* Do not print statistical information in file */
	error_ = 0;			/* Initialize error flag */

	/* ----------------------------------------------------------------*/
	/* .. Initialize the internal solver memory pointer. This is only  */
	/*   necessary for the FIRST call of the PARDISO solver.           */
	/* ----------------------------------------------------------------*/
	for (MKL_INT i = 0; i < 64; i++)
	{
		pt_[i] = 0;
	}
}

PardisoSolver::~PardisoSolver()
{
	/* --------------------------------------*/
	/* .. Termination and release of memory. */
	/* --------------------------------------*/

	/* Release internal memory. */
	phase_ = -1;
	pardiso(pt_, &maxfct_, &mnum_, &mtype_, &phase_, &n_, &ddum_, ia_.get(), ja_.get(), &idum_, &nrhs_, iparm_, &msglvl_, &ddum_, &ddum_, &error_);
}

void PardisoSolver::AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A)
{

}

void PardisoSolver::AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A)
{
	n_ = A.rows();
	ia_ = std::make_unique<MKL_INT[]>(A.outerSize() + 1);
	ja_ = std::make_unique<MKL_INT[]>(A.nonZeros());
	a_ = std::make_unique<double[]>(A.nonZeros());

	MKL_INT* ia = const_cast<MKL_INT*>(A.outerIndexPtr());
	MKL_INT* ja = const_cast<MKL_INT*>(A.innerIndexPtr());
	double* a = const_cast<double*>(A.valuePtr());

	#pragma omp parallel for
	for (MKL_INT i = 0; i < A.outerSize() + 1; i++)
	{
		ia_[i] = ia[i];
	}

	#pragma omp parallel for
	for (MKL_INT i = 0; i < A.nonZeros(); i++)
	{
		ja_[i] = ja[i];
	}

	#pragma omp parallel for
	for (MKL_INT i = 0; i < A.nonZeros(); i++)
	{
		a_[i] = 0;
	}

	// TODO: Make sure that the main diagonal is explicitly specified (even with zeros if needed) and that the lower triangular part is omitted (in case of symmetric matrix)

	/* --------------------------------------------------------------------*/
	/* .. Reordering and Symbolic Factorization. This step also allocates  */
	/*    all memory that is necessary for the factorization.              */
	/* --------------------------------------------------------------------*/
	phase_ = 11;
	pardiso(pt_, &maxfct_, &mnum_, &mtype_, &phase_, &n_, a_.get(), ia_.get(), ja_.get(), &idum_, &nrhs_, iparm_, &msglvl_, &ddum_, &ddum_, &error_);
}

void PardisoSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{

}

void PardisoSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	double* a = const_cast<double*>(A.valuePtr());

	#pragma omp parallel for
	for (MKL_INT i = 0; i < A.nonZeros(); i++)
	{
		a_[i] = a[i];
	}

	/* ----------------------------*/
	/* .. Numerical factorization. */
	/* ----------------------------*/
	phase_ = 22;
	pardiso(pt_, &maxfct_, &mnum_, &mtype_, &phase_, &n_, a_.get(), ia_.get(), ja_.get(), &idum_, &nrhs_, iparm_, &msglvl_, &ddum_, &ddum_, &error_);

	/* -----------------------------------------------*/
	/* .. Back substitution and iterative refinement. */
	/* -----------------------------------------------*/
	phase_ = 33;
	pardiso(pt_, &maxfct_, &mnum_, &mtype_, &phase_, &n_, a_.get(), ia_.get(), ja_.get(), &idum_, &nrhs_, iparm_, &msglvl_, const_cast<double*>(b.data()), const_cast<double*>(x.data()), &error_);
}