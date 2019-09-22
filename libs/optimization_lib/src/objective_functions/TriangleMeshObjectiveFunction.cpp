#include <objective_functions/TriangleMeshObjectiveFunction.h>

void TriangleMeshObjectiveFunction::init_mesh(const MatrixXd& V, const MatrixX3i& F) {
	MatrixX3d V3d(V.rows(), 3);
	if (V.cols() == 2) {
		V3d.leftCols(2) = V;
		V3d.col(2).setZero();
	}
	else if (V.cols() == 3) {
		V3d = V;
	}
	this->V = V3d;
	this->F = F;
}

void TriangleMeshObjectiveFunction::AddElementToHessian(std::vector<int> ind)
{
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	for (int i = 0; i < ind.size(); i++)
		for (int j = 0; j <= i; j++)
			PushPair(ind[i], ind[j]);
}