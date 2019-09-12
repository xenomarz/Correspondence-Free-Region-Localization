#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	
	ArrayXXd m(2, 1);
	m << 0.5, 0.5;
	cout << (m.asin())*(180/M_PI) << endl;
	


	return 0;

}
