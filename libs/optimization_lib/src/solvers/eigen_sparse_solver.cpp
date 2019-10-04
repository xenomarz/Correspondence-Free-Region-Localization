// Optimization lib includes
#include <solvers/eigen_sparse_solver.h>
#include <utils/utils.h>

EigenSparseSolver::EigenSparseSolver()
{

}

EigenSparseSolver::~EigenSparseSolver()
{

}

void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	// Compute the ordering permutation vector from the structural pattern of A
	solver_.analyzePattern(A);

	// Compute the numerical factorization 
	solver_.factorize(A);

	// Use the factors to solve the linear system 
	x = solver_.solve(b);
}

void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	// Compute the ordering permutation vector from the structural pattern of A
	solver_.analyzePattern(A);

	// Compute the numerical factorization 
	solver_.factorize(A);

	// Use the factors to solve the linear system 
	x = solver_.solve(b);
}