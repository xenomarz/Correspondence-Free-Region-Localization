#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	
	MatrixXd B;
	Eigen::VectorXd v(3);
	v << 1, 2, 3;
	

	B = v.replicate(1, 10).transpose();

	std::cout << B << "\n\n";

	return 0;

}
