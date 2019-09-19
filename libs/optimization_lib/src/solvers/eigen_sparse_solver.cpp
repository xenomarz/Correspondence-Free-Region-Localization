// Optimization lib includes
#include <solvers/eigen_sparse_solver.h>
#include <utils/utils.h>

EigenSparseSolver::EigenSparseSolver()
{

}

EigenSparseSolver::~EigenSparseSolver()
{

}

void EigenSparseSolver::Solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	// Compute the ordering permutation vector from the structural pattern of A
	solver_.analyzePattern(A);

	// Compute the numerical factorization 
	solver_.factorize(A);

	// Use the factors to solve the linear system 
	x = solver_.solve(b);
}

void EigenSparseSolver::Solve(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, const Eigen::VectorXd& b, Eigen::VectorXd& x)
{
	Eigen::SparseMatrix<double> A;
	Utils::SparseMatrixFromTriplets(ii, jj, ss, A);
	A.makeCompressed();
	Solve(A, b, x);
}