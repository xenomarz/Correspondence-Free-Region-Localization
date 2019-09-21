#include "../../../libs/optimization_lib/include/utils.h"


int GetUpperTriangleSize(int MatrixSize) {
	if (MatrixSize <= 0)
		return 0;
	return MatrixSize + GetUpperTriangleSize(MatrixSize - 1);
}

int main(int argc, char * argv[])
{
	cout << GetUpperTriangleSize(0) << endl;
	cout << GetUpperTriangleSize(1) << endl;
	cout << GetUpperTriangleSize(2) << endl;
	cout << GetUpperTriangleSize(3) << endl;
	cout << GetUpperTriangleSize(4) << endl;
	cout << GetUpperTriangleSize(5) << endl;
	cout << GetUpperTriangleSize(6) << endl;
	return 0;
}
