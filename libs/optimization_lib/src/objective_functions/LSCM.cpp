#include <objective_functions/LSCM.h>

LSCM::LSCM()
{
	name = "LSCM";
	w = 0;
}

void LSCM::init()
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
			Dx	, zero	,
			zero, Dx	,
			Dy	, zero	,
			zero, Dy;
	}

	init_hessian();
}

void LSCM::updateX(const VectorXd& X)
{
	bool inversions_exist = update_variables(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double LSCM::value(bool update)
{
	VectorXd E = 2*(d.cwiseAbs2()) + (b+c).cwiseAbs2() + 2 * (a.cwiseAbs2());
	double value = (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = value;
	}
	
	return value;
}

void LSCM::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		VectorXd gi;
		gi.resize(6);
		gi = Area(fi)*(grad.row(fi));

		//Update the gradient of the x-axis
		g(F(fi, 0)) += gi(0);
		g(F(fi, 1)) += gi(1);
		g(F(fi, 2)) += gi(2);
		//Update the gradient of the y-axis
		g(F(fi, 0) + V.rows()) += gi(3);
		g(F(fi, 1) + V.rows()) += gi(4);
		g(F(fi, 2) + V.rows()) += gi(5);
	}
	gradient_norm = g.norm();
}

void LSCM::hessian()
{
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
		
		Matrix<double, 6, 6> Hi = Area(i)*Hessian[i];

		int index2 = i * 21;
		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[index2++] = Hi(a, b);
			}
		}
	}
}

bool LSCM::update_variables(const VectorXd& X)
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
		double detj_1 = (a(i) * d(i) - b(i) * c(i)) - 1;

		//prepare gradient
		Vector4d dE_dJ(4 * a(i), 2 * b(i) + 2 * c(i), 2 * b(i) + 2 * c(i), 4 * d(i));
		grad.row(i) = (dE_dJ.transpose() * dJ_dX[i]).transpose();

		//prepare hessian
		MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			4, 0, 0, 0,
			0, 2, 2, 0,
			0, 2, 2, 0,
			0, 0, 0, 4;

		Hessian[i] = dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
	}
	detJ = a.cwiseProduct(d) - b.cwiseProduct(c);
	
	return ((detJ.array() < 0).any());
}

void LSCM::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	for (int i = 0; i < F.rows(); ++i)
	{
		// For every face there is a 6x6 local hessian.
		// We only need the 21 values contained in the upper triangle.
		// They are access and also put into the big hessian in column order. 

		// First column
		PushPair(F(i, 0)			, F(i, 0));

		// Second column
		PushPair(F(i, 0)			, F(i, 1));
		PushPair(F(i, 1)			, F(i, 1));

		// Third column
		PushPair(F(i, 0)			, F(i, 2));
		PushPair(F(i, 1)			, F(i, 2));
		PushPair(F(i, 2)			, F(i, 2));

		// Fourth column
		PushPair(F(i, 0)			, F(i, 0) + V.rows());
		PushPair(F(i, 1)			, F(i, 0) + V.rows());
		PushPair(F(i, 2)			, F(i, 0) + V.rows());
		PushPair(F(i, 0) + V.rows()	, F(i, 0) + V.rows());

		// Fifth column
		PushPair(F(i, 0)			, F(i, 1) + V.rows());
		PushPair(F(i, 1)			, F(i, 1) + V.rows());
		PushPair(F(i, 2)			, F(i, 1) + V.rows());
		PushPair(F(i, 0) + V.rows()	, F(i, 1) + V.rows());
		PushPair(F(i, 1) + V.rows()	, F(i, 1) + V.rows());

		// Sixth column
		PushPair(F(i, 0)			, F(i, 2) + V.rows());
		PushPair(F(i, 1)			, F(i, 2) + V.rows());
		PushPair(F(i, 2)			, F(i, 2) + V.rows());
		PushPair(F(i, 0) + V.rows()	, F(i, 2) + V.rows());
		PushPair(F(i, 1) + V.rows()	, F(i, 2) + V.rows());
		PushPair(F(i, 2) + V.rows()	, F(i, 2) + V.rows());
	}
	SS = vector<double>(II.size(), 0.);
}