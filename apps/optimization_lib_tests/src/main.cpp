#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
		
	double detj_1 = 10;
	
	Vector3d left = Vector3d(1, 2, 3);
	left *= detj_1;
	
	Vector3d right = Vector3d(4, 5, 6);
	right *= detj_1;

	VectorXd grad;
	grad.resize(6);
	grad << left, right;

	cout << grad << endl;

	return EXIT_SUCCESS;
}
