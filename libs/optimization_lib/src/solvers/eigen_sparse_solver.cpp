// Optimization lib includes
#include <solvers/eigen_sparse_solver.h>
#include <utils/utils.h>

EigenSparseSolver::EigenSparseSolver()
{

}

EigenSparseSolver::~EigenSparseSolver()
{

}

void EigenSparseSolver::AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A)
{
	solver_.analyzePattern(A);
}

void EigenSparseSolver::AnalyzePattern(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A)
{
	solver_.analyzePattern(A);
}

void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::ColMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	// Compute the numerical factorization 
	solver_.factorize(A);

	// Use the factors to solve the linear system 
	x = solver_.solve(b);
}

void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double, Eigen::StorageOptions::RowMajor>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	// Compute the numerical factorization 
	solver_.factorize(A);

	// Use the factors to solve the linear system 
	x = solver_.solve(b);
}