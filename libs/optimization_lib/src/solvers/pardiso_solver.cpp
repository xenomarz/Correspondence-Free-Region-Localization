// Optimization lib includes
#include <solvers/pardiso_solver.h>
#include <utils/utils.h>

// MKL includes
#include "mkl_pardiso.h"
#include "mkl_types.h"

PardisoSolver::PardisoSolver()
{

}

PardisoSolver::~PardisoSolver()
{

}

void PardisoSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{

}

void PardisoSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	auto A_copy = A;
	for (Eigen::DenseIndex i = 0; i < A.rows(); i++)
	{
		A_copy.coeffRef(i, i) = A_copy.coeffRef(i, i) + 0;
	}

	A_copy.makeCompressed();

	/* Matrix data. */
	MKL_INT n = A_copy.rows();
	MKL_INT* ia = A_copy.outerIndexPtr();
	MKL_INT* ja = A_copy.innerIndexPtr();
	double* a = A_copy.valuePtr();

	/* Real symmetric matrix */
	MKL_INT mtype = -2;

	/* Number of right hand sides. */
	MKL_INT nrhs = 1;

	/* Internal solver memory pointer pt, */
	/* 32-bit: int pt[64]; 64-bit: long int pt[64] */
	/* or void *pt[64] should be OK on both architectures */
	void* pt[64];

	/* Pardiso control parameters. */
	MKL_INT iparm[64];
	MKL_INT maxfct, mnum, phase, error, msglvl;

	/* Auxiliary variables. */
	MKL_INT i;

	/* Double dummy */
	double ddum;

	/* Integer dummy. */
	MKL_INT idum;

	/* -------------------------------------*/
	/* .. Setup Pardiso control parameters. */
	/* -------------------------------------*/
	for (i = 0; i < 64; i++)
	{
		iparm[i] = 0;
	}

	iparm[0] = 1;		/* No solver default */
	iparm[1] = 2;		/* Fill-in reordering from METIS */
	iparm[3] = 0;		/* No iterative-direct algorithm */
	iparm[4] = 0;		/* No user fill-in reducing permutation */
	iparm[5] = 0;		/* Write solution into x */
	iparm[7] = 2;		/* Max numbers of iterative refinement steps */
	iparm[9] = 13;		/* Perturb the pivot elements with 1E-13 */
	iparm[10] = 1;		/* Use nonsymmetric permutation and scaling MPS */
	iparm[12] = 0;		/* Maximum weighted matching algorithm is switched-off (default for symmetric). Try iparm[12] = 1 in case of inappropriate accuracy */
	iparm[13] = 0;		/* Output: Number of perturbed pivots */
	iparm[17] = -1;		/* Output: Number of nonzeros in the factor LU */
	iparm[18] = -1;		/* Output: Mflops for LU factorization */
	iparm[19] = 0;		/* Output: Numbers of CG Iterations */
	iparm[34] = 1;		/* PARDISO use C-style indexing for ia and ja arrays */
	maxfct = 1;			/* Maximum number of numerical factorizations. */
	mnum = 1;			/* Which factorization to use. */
	msglvl = 1;			/* Print statistical information in file */
	error = 0;			/* Initialize error flag */

	/* ----------------------------------------------------------------*/
	/* .. Initialize the internal solver memory pointer. This is only  */
	/*   necessary for the FIRST call of the PARDISO solver.           */
	/* ----------------------------------------------------------------*/
	for (i = 0; i < 64; i++)
	{
		pt[i] = 0;
	}

	/* --------------------------------------------------------------------*/
	/* .. Reordering and Symbolic Factorization. This step also allocates  */
	/*    all memory that is necessary for the factorization.              */
	/* --------------------------------------------------------------------*/
	phase = 11;
	PARDISO(pt, &maxfct, &mnum, &mtype, &phase, &n, a, ia, ja, &idum, &nrhs, iparm, &msglvl, &ddum, &ddum, &error);

	/* ----------------------------*/
	/* .. Numerical factorization. */
	/* ----------------------------*/
	phase = 22;
	PARDISO(pt, &maxfct, &mnum, &mtype, &phase, &n, a, ia, ja, &idum, &nrhs, iparm, &msglvl, &ddum, &ddum, &error);

	/* -----------------------------------------------*/
	/* .. Back substitution and iterative refinement. */
	/* -----------------------------------------------*/
	phase = 33;

	/* Max numbers of iterative refinement steps. */
	iparm[7] = 2;

	PARDISO(pt, &maxfct, &mnum, &mtype, &phase, &n, a, ia, ja, &idum, &nrhs, iparm, &msglvl, const_cast<double*>(b.data()), const_cast<double*>(x.data()), &error);

	/* --------------------------------------*/
	/* .. Termination and release of memory. */
	/* --------------------------------------*/

	/* Release internal memory. */
	phase = -1;
	PARDISO(pt, &maxfct, &mnum, &mtype, &phase, &n, &ddum, ia, ja, &idum, &nrhs, iparm, &msglvl, &ddum, &ddum, &error);
}