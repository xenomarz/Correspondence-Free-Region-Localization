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

void PardisoSolver::Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{

}

void PardisoSolver::Solve(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	Eigen::SparseMatrix<double, Eigen::RowMajor> A;
	Utils::SparseMatrixFromTriplets<Eigen::RowMajor>(ii, jj, ss, A);
	A.makeCompressed();
	Solve(A, b, x);
}