#include "../../../libs/optimization_lib/include/utils.h"


int GetUpperTriangleSize(int MatrixSize) {
	if (MatrixSize <= 0)
		return 0;
	return MatrixSize + GetUpperTriangleSize(MatrixSize - 1);
}

int main(int argc, char * argv[])
{
	VectorXd x; x.resize(9);
	int v = 3;
	x << 1, 1, 1, 2, 2, 2, 3, 3, 3;
	cout << x.head(v * 2);
	cout << x.tail(v);
	return 0;
}
