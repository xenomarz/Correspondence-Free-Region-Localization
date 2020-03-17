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
	perpareMatrix(II, JJ, SS);
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
	perpareMatrix(II,JJ,SS);
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
void EigenSparseSolver<vectorTypeI, vectorTypeS>::perpareMatrix(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS)
{
	UpperTriangular_A = Utils::BuildMatrix(II,JJ,SS);
	full_A = UpperTriangular_A.selfadjointView<Eigen::Upper>();

	


	auto eig = full_A.toDense().eigenvalues();
	assert(eig.imag().isZero() && "A complex eigenvalue number has been found!");
	double min_eig_value = eig.real().minCoeff();
	
	cout << "before: full min_eig_value = " << min_eig_value << endl;
	cout << "before: full max_eig_value = " << eig.real().maxCoeff() << endl;
	cout << "before: upper min_eig_value = " << UpperTriangular_A.toDense().eigenvalues().real().minCoeff()<< endl;
	cout << "before: upper max_eig_value = " << UpperTriangular_A.toDense().eigenvalues().real().maxCoeff()<< endl;
	
	if (min_eig_value < 0) {
		for (int i = 0; i < full_A.rows(); i++) {
			full_A.coeffRef(i, i) = full_A.coeff(i, i) + (-min_eig_value + 1e-4);
		}
	}

	cout << "after: full min_eig_value = " << full_A.toDense().eigenvalues().real().minCoeff() << endl;
	cout << "after: full max_eig_value = " << full_A.toDense().eigenvalues().real().maxCoeff() << endl;
	cout << "after: upper min_eig_value = " << UpperTriangular_A.toDense().eigenvalues().real().minCoeff() << endl;
	cout << "after: upper max_eig_value = " << UpperTriangular_A.toDense().eigenvalues().real().maxCoeff() << endl;


	

	full_A.makeCompressed();
	UpperTriangular_A.makeCompressed();
}

template class EigenSparseSolver<std::vector<int, std::allocator<int> >, std::vector<double, std::allocator<double> > >;
