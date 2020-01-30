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
	//solver.factorize(full_A/*UpperTriangular_A*/);
	//assert(solver.info() == Eigen::Success && "factorization failed!");
	//return solver.info() == 0;

	// Launch MATLAB
	igl::matlab::mlinit(&engine);
	igl::matlab::mleval(&engine, "desktop");

	// Send matrix to matlab
	igl::matlab::mlsetmatrix(&engine, "A", full_A);
	igl::matlab::mleval(&engine, "A = full(A);");
	return true;
}

template <typename vectorTypeI, typename vectorTypeS>
Eigen::VectorXd EigenSparseSolver<vectorTypeI, vectorTypeS>::solve(Eigen::VectorXd &rhs)
{
	Eigen::VectorXd x;
	//x = solver.solve(rhs);

	Eigen::MatrixXd temp;
	igl::matlab::mlsetmatrix(&engine, "b", Eigen::MatrixXd(rhs));
	igl::matlab::mleval(&engine, "x = A\\b;");
	//igl::matlab::mleval(&engine, "x = symmlq(A,b);");
	igl::matlab::mlgetmatrix(&engine, "x",temp);
	x = temp;


	MSE = (full_A * x - rhs).cwiseAbs2().sum();
	return x;
}

template <typename vectorTypeI, typename vectorTypeS>
void EigenSparseSolver<vectorTypeI, vectorTypeS>::BuildMatrix(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	// Hessian sparse reprensentation
	std::vector<int> fullII, fullJJ;
	std::vector<double> fullSS;

	assert(II.size() == JJ.size() && II.size() == SS.size() && "II == JJ at Newton internal init");
	for (int i = 0; i < II.size(); i++) {
		fullII.push_back(II[i]);
		fullJJ.push_back(JJ[i]);
		fullSS.push_back(SS[i]);

		//add the lower triangular values
		if (II[i] < JJ[i]) {
			fullII.push_back(JJ[i]);
			fullJJ.push_back(II[i]);
			fullSS.push_back(SS[i]);
		}
		assert(II[i] <= JJ[i] && "Error!");
	}
	assert(fullII.size() == fullJJ.size() && fullII.size() == fullSS.size() && "fullII == fullJJ at Newton internal init");
	

	std::vector<Eigen::Triplet<double>> tripletList1;
	tripletList1.reserve(fullII.size());
	int rows1 = *std::max_element(fullII.begin(), fullII.end()) + 1;
	int cols1 = *std::max_element(fullJJ.begin(), fullJJ.end()) + 1;
	assert(rows1 == cols1 && "Rows == Cols at Newton internal init");
	for (int i = 0; i < fullII.size(); i++)
		tripletList1.push_back(Eigen::Triplet<double>(fullII[i], fullJJ[i], fullSS[i]));
	full_A.resize(rows1, cols1);
	full_A.setFromTriplets(tripletList1.begin(), tripletList1.end());

	//////////////////////////////////
	std::vector<Eigen::Triplet<double>> tripletList2;
	tripletList2.reserve(II.size());
	int rows2 = *std::max_element(II.begin(), II.end()) + 1;
	int cols2 = *std::max_element(JJ.begin(), JJ.end()) + 1;
	assert(rows2 == cols2 && "Rows == Cols at Newton internal init");
	for (int i = 0; i < II.size(); i++)
		tripletList2.push_back(Eigen::Triplet<double>(II[i], JJ[i], SS[i]));
	UpperTriangular_A.resize(rows2, cols2);
	UpperTriangular_A.setFromTriplets(tripletList2.begin(), tripletList2.end());
	//////////////////////////////////
}

template class EigenSparseSolver<std::vector<int, std::allocator<int> >, std::vector<double, std::allocator<double> > >;

//template class eigen_sparse_solver<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >;