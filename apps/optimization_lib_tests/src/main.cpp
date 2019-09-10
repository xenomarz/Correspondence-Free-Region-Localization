#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	MatrixXd a(3,3);
	a << 1, 2, -5,
		4, 5, 0,
		7, 8, 0;
	cout << a << endl;
	cout << (a.col(2).array() == 0).all() << endl;
	
	return 0;

}
