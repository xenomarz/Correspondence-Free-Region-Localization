#include "solvers/EigenSparseSolver.h"
#include <vector>
#include <iostream>
using namespace std;

template <typename vectorTypeI, typename vectorTypeS>
EigenSparseSolver<vectorTypeI, vectorTypeS>::EigenSparseSolver()
{
}

template <typename vectorTypeI, typename vectorTypeS>
EigenSparseSolver<vectorTypeI, vectorTypeS>::~EigenSparseSolver()
{
}

template <typename vectorTypeI, typename vectorTypeS>
void EigenSparseSolver<vectorTypeI, vectorTypeS>::set_pattern(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	BuildMatrix(II, JJ, SS);
}

template <typename vectorTypeI, typename vectorTypeS>
void EigenSparseSolver<vectorTypeI, vectorTypeS>::analyze_pattern()
{
	solver.analyzePattern(full_A/*UpperTriangular_A*/);
	//assert(solver.info() == Eigen::Success && "analyzePattern failed!");
}

template <typename vectorTypeI, typename vectorTypeS>
bool EigenSparseSolver<vectorTypeI, vectorTypeS>::factorize(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	BuildMatrix(II,JJ,SS);
	solver.factorize(full_A/*UpperTriangular_A*/);
	assert(solver.info() == Eigen::Success && "factorization failed!");
	return solver.info() == 0;

	//// Launch MATLAB
	//igl::matlab::mlinit(&engine);
	//igl::matlab::mleval(&engine, "desktop");

	//// Send matrix to matlab
	//igl::matlab::mlsetmatrix(&engine, "A", full_A);
	//igl::matlab::mleval(&engine, "A = full(A);");
	//return true;
}

template <typename vectorTypeI, typename vectorTypeS>
Eigen::VectorXd EigenSparseSolver<vectorTypeI, vectorTypeS>::solve(Eigen::VectorXd &rhs)
{
	Eigen::VectorXd x;
	x = solver.solve(rhs);

	/*Eigen::MatrixXd temp;
	igl::matlab::mlsetmatrix(&engine, "b", Eigen::MatrixXd(rhs));
	igl::matlab::mleval(&engine, "x = A\\b;");
	igl::matlab::mlgetmatrix(&engine, "x",temp);
	x = temp;*/


	MSE = (full_A * x - rhs).cwiseAbs2().sum();
	return x;
}

template <typename vectorTypeI, typename vectorTypeS>
void EigenSparseSolver<vectorTypeI, vectorTypeS>::BuildMatrix(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	assert(II.size() == JJ.size() && II.size() == SS.size() && "II == JJ at Eigen solver internal init");
	std::vector<Eigen::Triplet<double>> tripletList;
	tripletList.reserve(II.size());
	int rows = *std::max_element(II.begin(), II.end()) + 1;
	int cols = *std::max_element(JJ.begin(), JJ.end()) + 1;
	assert(rows == cols && "Rows == Cols at Newton internal init");
	for (int i = 0; i < II.size(); i++)
		tripletList.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	UpperTriangular_A.resize(rows, cols);
	UpperTriangular_A.setFromTriplets(tripletList.begin(), tripletList.end());
	
	full_A = UpperTriangular_A.selfadjointView<Eigen::Upper>();

	full_A.makeCompressed();
	UpperTriangular_A.makeCompressed();
}

template class EigenSparseSolver<std::vector<int, std::allocator<int> >, std::vector<double, std::allocator<double> > >;
