#include "../../../libs/optimization_lib/include/utils.h"
#include "../../../libs/optimization_lib/include/solvers/EigenSparseSolver.h"

#include <Eigen/SparseLU>
#include <iostream>
#include <fstream>
using namespace std;
int main(int argc, char * argv[])
{
	EigenSparseSolver<std::vector<int>, std::vector<double>> eigen_solver;
	vector<int> II, JJ;
	vector<double> SS;
	Eigen::VectorXd b(2); b << 1 ,5;

	II.clear();	JJ.clear(); SS.clear();
	II.push_back(0); JJ.push_back(0); SS.push_back(1);
	II.push_back(0); JJ.push_back(1); SS.push_back(2);
	II.push_back(1); JJ.push_back(1); SS.push_back(3);

	eigen_solver.set_pattern(II, JJ, SS);
	eigen_solver.analyze_pattern();
	eigen_solver.factorize(II, JJ, SS);
	eigen_solver.solve(b);

	//Eigen::SparseMatrix<double> A1(2,2);
	//A1.insert(0, 0) = 1; A1.insert(0, 1) = 2;
	//A1.insert(1, 0) = 2; A1.insert(1, 1) = 3;
	//A1.makeCompressed();

	//Eigen::SparseMatrix<double> A2(2, 2);
	//A2.insert(0, 0) = 1; A2.insert(0, 1) = 2;
	//A2.insert(1, 0) = 0; A2.insert(1, 1) = 3;
	//A2.makeCompressed();


	//Eigen::VectorXd b(2), x1,x2;
	//b << 1 ,5;
	//
	//// solve A1 * x1 = b
	//Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
	//solver.analyzePattern(A1);
	//solver.factorize(A1);
	//if (solver.info() != Eigen::Success) {
	//	// decomposition failed
	//	return -1;
	//}
	//x1 = solver.solve(b);
	//if (solver.info() != Eigen::Success) {
	//	// solving failed
	//	return -2;
	//}

	//solver.analyzePattern(A2);
	//solver.factorize(A2);
	//if (solver.info() != Eigen::Success) {
	//	// decomposition failed
	//	return -1;
	//}
	//x2 = solver.solve(b);
	//if (solver.info() != Eigen::Success) {
	//	// solving failed
	//	return -2;
	//}

	//
	//cout << "A1 = " << endl << A1.toDense() << endl;
	//cout << "x1 = " << endl << x1 << endl;
	//cout << "b = " << endl << b << endl;

	//cout << "A2 = " << endl << A2.toDense() << endl;
	//cout << "x2 = " << endl << x2 << endl;
	//cout << "b = " << endl << b << endl;


	
	return 0;
}
