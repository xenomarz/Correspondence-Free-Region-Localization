#include <objective_functions/TriangleMeshObjectiveFunction.h>

void TriangleMeshObjectiveFunction::init()
{
	if (V.size() == 0 || F.size() == 0)
		throw name + " must define members V,F before init()!";

	a.resize(F.rows());
	b.resize(F.rows());
	c.resize(F.rows());
	d.resize(F.rows());

	//Parameterization J mats resize
	detJ.resize(F.rows());
	detJ.resize(F.rows());

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	MatrixX3d D1cols, D2cols;

	Utils::computeSurfaceGradientPerFace(V, F, D1cols, D2cols);
	D1d = D1cols.transpose();
	D2d = D2cols.transpose();

	//prepare dJ/dX
	dJ_dX.resize(F.rows());
	for (int i = 0; i < F.rows(); i++) {
		MatrixXd Dx = D1d.col(i).transpose();
		MatrixXd Dy = D2d.col(i).transpose();
		MatrixXd zero = VectorXd::Zero(3).transpose();
		dJ_dX[i] <<
			Dx, zero,
			zero, Dx,
			Dy, zero,
			zero, Dy;
	}
}

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