#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

int main(int argc, char * argv[])
{
	MatrixXd Length_output(2,3), Length_input(2, 3), DistortionPerFace(2, 3);
	Length_output << 2, 4, 6, 8, 20, 40;
	Length_input << 1, 2, 3, 4, 10, 20;
	
	cout << "Length_output = " << endl << Length_output << endl;
	cout << "Length_input = " << endl << Length_input << endl;
	cout << "Length_input.cwiseInverse() = " << endl << Length_input.cwiseInverse() << endl;

	DistortionPerFace = Length_input.cwiseInverse().cwiseProduct(Length_output);
	cout << "DistortionPerFace = " << endl << DistortionPerFace << endl;
	VectorXd a = DistortionPerFace.rowwise().sum() / 3;
	cout << "a = " << endl << a << endl;
	


	return 0;

}
