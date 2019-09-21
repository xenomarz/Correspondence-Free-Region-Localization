#include "../../../libs/optimization_lib/include/utils.h"


int main(int argc, char * argv[])
{
	
	
	
	vector<int> OneRingFaces;
	OneRingFaces.push_back(5);
	OneRingFaces.push_back(54);
	OneRingFaces.push_back(22);
	
	
	int J_size = 4 * OneRingFaces.size();
	int X_size = 6 * OneRingFaces.size();

	MatrixXd dJ_dX2_curr(J_size, X_size);
	dJ_dX2_curr.setZero();
	for (int i = 0; i < OneRingFaces.size(); i++) {
		int fi = OneRingFaces[i];
		int base_row = 4 * i;
		int base_column = 6 * i;

		RowVectorXd Dx = VectorXd::Constant(3,1, fi).transpose();
		RowVectorXd Dy = VectorXd::Constant(3,1, fi+1).transpose();
		RowVectorXd zero = VectorXd::Zero(3).transpose();
		dJ_dX2_curr.block<4, 6>(base_row, base_column) <<
			Dx, zero,
			zero, Dx,
			Dy, zero,
			zero, Dy;
	}

	/*dJ_dX2.push_back(dJ_dX2_curr);*/
	cout << dJ_dX2_curr;
	

	return 0;
}
