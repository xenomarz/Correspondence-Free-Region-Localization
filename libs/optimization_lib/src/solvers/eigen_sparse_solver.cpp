#include <solvers/eigen_sparse_solver.h>
#include <vector>
#include <iostream>
using namespace std;

template <typename vectorTypeI, typename vectorTypeS>
eigen_sparse_solver<vectorTypeI, vectorTypeS>::eigen_sparse_solver()
{
}

template <typename vectorTypeI, typename vectorTypeS>
eigen_sparse_solver<vectorTypeI, vectorTypeS>::~eigen_sparse_solver()
{
}

template <typename vectorTypeI, typename vectorTypeS>
void eigen_sparse_solver<vectorTypeI, vectorTypeS>::set_pattern(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	std::vector<Eigen::Triplet<double>> tripletList;
	tripletList.reserve(II.size());
	int rows = *std::max_element(II.begin(), II.end()) + 1;
	int cols = *std::max_element(JJ.begin(), JJ.end()) + 1;
	assert(rows == cols && "Rows == Cols at Newton internal init");
	for (int i = 0; i<II.size(); i++)
		tripletList.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	A.resize(rows, cols);
	A.setFromTriplets(tripletList.begin(), tripletList.end());
}

template <typename vectorTypeI, typename vectorTypeS>
void eigen_sparse_solver<vectorTypeI, vectorTypeS>::analyze_pattern()
{
	solver.analyzePattern(A);
}

template <typename vectorTypeI, typename vectorTypeS>
bool eigen_sparse_solver<vectorTypeI, vectorTypeS>::factorize(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	std::vector<Eigen::Triplet<double>> tripletList;
	tripletList.reserve(II.size());
	int rows = *std::max_element(II.begin(), II.end()) + 1;
	int cols = *std::max_element(JJ.begin(), JJ.end()) + 1;
	assert(rows == cols && "Rows == Cols at Newton internal init");
	for (int i = 0; i<II.size(); i++)
		tripletList.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	A.resize(rows, cols);
	A.setFromTriplets(tripletList.begin(), tripletList.end());
	solver.factorize(A);
    return solver.info() == 0;
}

template <typename vectorTypeI, typename vectorTypeS>
Eigen::VectorXd eigen_sparse_solver<vectorTypeI, vectorTypeS>::solve(Eigen::VectorXd &rhs)
{
	return solver.solve(rhs);
}

template class eigen_sparse_solver<std::vector<int, std::allocator<int> >, std::vector<double, std::allocator<double> > >;

//template class eigen_sparse_solver<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >;