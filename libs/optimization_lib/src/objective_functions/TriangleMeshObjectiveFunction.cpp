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
	grad.resize(F.rows(), 6);
	Hessian.resize(F.rows());
	dJ_dX.resize(F.rows());

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	MatrixX3d D1cols, D2cols;

	Utils::computeSurfaceGradientPerFace(V, F, D1cols, D2cols);
	D1d = D1cols.transpose();
	D2d = D2cols.transpose();

	//prepare dJ/dX
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

	init_hessian();
}

bool TriangleMeshObjectiveFunction::update_variables(const VectorXd& X)
{
	Eigen::Map<const MatrixX2d> x(X.data(), X.size() / 2, 2);
	for (int i = 0; i < F.rows(); i++)
	{
		Vector3d Xi, Yi;
		Xi << x(F(i, 0), 0), x(F(i, 1), 0), x(F(i, 2), 0);
		Yi << x(F(i, 0), 1), x(F(i, 1), 1), x(F(i, 2), 1);
		Vector3d Dx = D1d.col(i);
		Vector3d Dy = D2d.col(i);
		//prepare jacobian		
		a(i) = Dx.transpose() * Xi;
		b(i) = Dx.transpose() * Yi;
		c(i) = Dy.transpose() * Xi;
		d(i) = Dy.transpose() * Yi;
		detJ(i) = a(i) * d(i) - b(i) * c(i);
	}
	return ((detJ.array() < 0).any());
}

void TriangleMeshObjectiveFunction::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToHessian({ F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });
	SS = vector<double>(II.size(), 0.);
}

void TriangleMeshObjectiveFunction::updateX(const VectorXd& X)
{
	bool inversions_exist = update_variables(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

void TriangleMeshObjectiveFunction::AddElementToHessian(std::vector<int> ind)
{
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	for (int i = 0; i < ind.size(); i++)
		for (int j = 0; j <= i; j++)
			PushPair(ind[i], ind[j]);
}