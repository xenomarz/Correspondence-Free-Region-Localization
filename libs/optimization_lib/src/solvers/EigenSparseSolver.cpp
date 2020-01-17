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
	solver.analyzePattern(A);
}

template <typename vectorTypeI, typename vectorTypeS>
bool EigenSparseSolver<vectorTypeI, vectorTypeS>::factorize(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	BuildMatrix(II,JJ,SS);
	solver.factorize(A);
    return solver.info() == 0;
}

template <typename vectorTypeI, typename vectorTypeS>
Eigen::VectorXd EigenSparseSolver<vectorTypeI, vectorTypeS>::solve(Eigen::VectorXd &rhs)
{
	Eigen::VectorXd x = solver.solve(rhs);
	MSE = (A * x - rhs).cwiseAbs2().sum();
	cout << "MSE = " << MSE << endl;
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
		if (II[i] != JJ[i]) {
			fullII.push_back(JJ[i]);
			fullJJ.push_back(II[i]);
			fullSS.push_back(SS[i]);
		}
	}
	assert(fullII.size() == fullJJ.size() && fullII.size() == fullSS.size() && "fullII == fullJJ at Newton internal init");

	
	std::vector<Eigen::Triplet<double>> tripletList;
	tripletList.reserve(fullII.size());
	int rows = *std::max_element(fullII.begin(), fullII.end()) + 1;
	int cols = *std::max_element(fullJJ.begin(), fullJJ.end()) + 1;
	assert(rows == cols && "Rows == Cols at Newton internal init");
	for (int i = 0; i < fullII.size(); i++)
		tripletList.push_back(Eigen::Triplet<double>(fullII[i], fullJJ[i], fullSS[i]));
	A.resize(rows, cols);
	A.setFromTriplets(tripletList.begin(), tripletList.end());

	///////////////////////////////
	//// Send matrix to matlab
	//// Launch MATLAB
	//igl::matlab::mlinit(&engine);
	//igl::matlab::mleval(&engine, "desktop");

	//igl::matlab::mlsetmatrix(&engine, "A", A);
	//igl::matlab::mleval(&engine, "A = full(A)");
	//igl::matlab::mleval(&engine, "A = A + A' - diag(diag(A))");

	//Eigen::MatrixXd sparse;
	//igl::matlab::mlgetmatrix(&engine, "A", sparse);
	//A = sparse.sparseView();
	///////////////////////////////
}

template class EigenSparseSolver<std::vector<int, std::allocator<int> >, std::vector<double, std::allocator<double> > >;

//template class eigen_sparse_solver<Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, 1, 0, -1, 1> >;