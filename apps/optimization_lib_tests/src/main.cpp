#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	//prepare hessian
	Matrix<double, 3, 3> dxx, dyy, dxy, dyx;
	Matrix<double, 6, 6> Hessian;

	dxx << 1, 2, 3, 4, 5, 6, 7, 8, 9;
	dyy << -1, -2, -3, -4, -5, -6, -7, -8, -9;
	dxy << 11, 12, 13, 14, 15, 16, 17, 18, 19;

	dyx = dxy.transpose();

	Hessian << dxx(0, 0), dxx(0, 1), dxx(0, 2), dxy(0, 0), dxy(0, 1), dxy(0, 2),
		dxx(1, 0), dxx(1, 1), dxx(1, 2), dxy(1, 0), dxy(1, 1), dxy(1, 2),
		dxx(2, 0), dxx(2, 1), dxx(2, 2), dxy(2, 0), dxy(2, 1), dxy(2, 2),

		dyx(0, 0), dyx(0, 1), dyx(0, 2), dyy(0, 0), dyy(0, 1), dyy(0, 2),
		dyx(1, 0), dyx(1, 1), dyx(1, 2), dyy(1, 0), dyy(1, 1), dyy(1, 2),
		dyx(2, 0), dyx(2, 1), dyx(2, 2), dyy(2, 0), dyy(2, 1), dyy(2, 2);


	cout << dxx << endl;
	cout << dxy << endl;
	cout << dyx << endl;
	cout << dyy << endl;
	cout << Hessian << endl;



	return EXIT_SUCCESS;
}
