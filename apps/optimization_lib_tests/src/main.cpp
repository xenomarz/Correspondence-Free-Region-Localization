#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	VectorXd a(4),b(4);

	a << 1, 2, -5, 7;
	b << 1, 3, 10, 17;
	cout << a << endl;
	cout << a.cwiseInverse().cwiseProduct(b) << endl;
	
	return 0;

}
