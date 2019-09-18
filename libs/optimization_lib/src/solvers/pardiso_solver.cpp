// Optimization lib includes
#include <solvers/pardiso_solver.h>

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