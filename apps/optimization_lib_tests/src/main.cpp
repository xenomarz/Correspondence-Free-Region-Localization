#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	
	MatrixXd B;
	Eigen::VectorXd v(3);
	v << 1, 2, 3;
	
	VectorXd alpha_vec(5), beta_vec(5);
	alpha_vec << 2, 2, 2, 2, 2;
	beta_vec << 1, 1, 1, 1, 1;

	VectorXd M_vec;
	M_vec.resize(3);
	M_vec << 1, 2, 3;
	VectorXd E_vec; E_vec.resize(3); E_vec << 4, 5, 6;
	MatrixXd M = M_vec.replicate(1, 5).transpose();
	MatrixXd E = E_vec.replicate(1, 5).transpose();
	MatrixXd alpha(5, 3), beta(5, 3);
	alpha.col(0) = alpha_vec;
	alpha.col(1) = alpha_vec;
	alpha.col(2) = alpha_vec;
	beta.col(0) = beta_vec;
	beta.col(1) = beta_vec;
	beta.col(2) = beta_vec;
	cout << "beta = " <<endl<< beta << endl;
	cout << "alpha = " << endl << alpha << endl;
	cout << "E = " << endl << E << endl;
	cout << "M = " << endl << M << endl;
	cout << "beta.cwiseProduct(M) + alpha.cwiseProduct(E) = " << endl << beta.cwiseProduct(M) + alpha.cwiseProduct(E) << endl;

	return 0;

}
