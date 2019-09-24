#include <objective_functions/symmetric_dirichlet.h>

symmetric_dirichlet::symmetric_dirichlet()
{
	name = "symmetric dirichlet";
	w = 0.1;
}

void symmetric_dirichlet::init()
{
	if (V.size() == 0 || F.size() == 0)
		throw name + " must define members V,F before init()!";
	
	a.resize(F.rows());
	b.resize(F.rows());
	c.resize(F.rows());
	d.resize(F.rows());
	
	//Parameterization J mats resize
	detJ.resize(F.rows());
	dirichlet.resize(F.rows());
	grad.resize(F.rows(), 6);
	Hessian.resize(F.rows());
	dJ_dX.resize(F.rows());

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	MatrixX3d D1cols, D2cols;

	utils::computeSurfaceGradientPerFace(V, F, D1cols, D2cols);
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

void symmetric_dirichlet::updateX(const VectorXd& X)
{
	bool inversions_exist = update_variables(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double symmetric_dirichlet::value(bool update)
{
	VectorXd invDirichlet = dirichlet.cwiseQuotient(detJ.cwiseAbs2());
	VectorXd E = dirichlet + invDirichlet;
	double value = 0.5 * (Area.asDiagonal() * E).sum();
	if (update) {
		Efi = E;
		energy_value = value;
	}

	return value;
}

void symmetric_dirichlet::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Vector4d dE_dJ;
		dE_dJ <<
			a(fi) + (a(fi) / pow(detJ(fi), 2)) - ((d(fi)*dirichlet(fi)) / pow(detJ(fi), 3)),
			b(fi) + (b(fi) / pow(detJ(fi), 2)) + ((c(fi)*dirichlet(fi)) / pow(detJ(fi), 3)),
			c(fi) + (c(fi) / pow(detJ(fi), 2)) + ((b(fi)*dirichlet(fi)) / pow(detJ(fi), 3)),
			d(fi) + (d(fi) / pow(detJ(fi), 2)) - ((a(fi)*dirichlet(fi)) / pow(detJ(fi), 3));

		grad.row(fi) = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();

		//Update the gradient of the x-axis
		g(F(fi, 0)) += grad(fi, 0);
		g(F(fi, 1)) += grad(fi, 1);
		g(F(fi, 2)) += grad(fi, 2);
		//Update the gradient of the y-axis
		g(F(fi, 0) + V.rows()) += grad(fi, 3);
		g(F(fi, 1) + V.rows()) += grad(fi, 4);
		g(F(fi, 2) + V.rows()) += grad(fi, 5);
	}
	gradient_norm = g.norm();
}

void symmetric_dirichlet::hessian()
{
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		MatrixXd d2E_dJ2(4, 4);
		double aa = 1
			+ (1 / pow(detJ(i), 2))
			- ((4 * a(i)*d(i)) / pow(detJ(i), 3))
			+ ((3 * pow(d(i), 2)*dirichlet(i)) / pow(detJ(i), 4));

		double bb = 1
			+ (1 / pow(detJ(i), 2))
			+ ((4 * b(i)*c(i)) / pow(detJ(i), 3))
			+ ((3 * pow(c(i), 2)*dirichlet(i)) / pow(detJ(i), 4));

		double cc = 1
			+ (1 / pow(detJ(i), 2))
			+ ((4 * b(i)*c(i)) / pow(detJ(i), 3))
			+ ((3 * pow(b(i), 2)*dirichlet(i)) / pow(detJ(i), 4));

		double dd = 1
			+ (1 / pow(detJ(i), 2))
			- ((4 * a(i)*d(i)) / pow(detJ(i), 3))
			+ ((3 * pow(a(i), 2)*dirichlet(i)) / pow(detJ(i), 4));

		double ab = (-3 * c(i)*d(i)*dirichlet(i))
			+ (2 * (a(i)*c(i) - b(i)*d(i))*detJ(i));
		ab /= pow(detJ(i), 4);

		double ac = (-3 * b(i)*d(i)*dirichlet(i))
			+ (2 * (a(i)*b(i) - c(i)*d(i))*detJ(i));
		ac /= pow(detJ(i), 4);

		double ad = (3 * a(i)*d(i)*dirichlet(i))
			- ((2 * pow(a(i), 2) + 2 * pow(d(i), 2) + dirichlet(i))*detJ(i));
		ad /= pow(detJ(i), 4);

		double bc = (3 * b(i)*c(i)*dirichlet(i))
			+ ((2 * pow(b(i), 2) + 2 * pow(c(i), 2) + dirichlet(i))*detJ(i));
		bc /= pow(detJ(i), 4);

		double bd = (-3 * a(i)*c(i)*dirichlet(i))
			+ (2 * (c(i)*d(i) - a(i)*b(i))*detJ(i));
		bd /= pow(detJ(i), 4);

		double cd = (-3 * a(i)*b(i)*dirichlet(i))
			+ (2 * (b(i)*d(i) - a(i)*c(i))*detJ(i));;
		cd /= pow(detJ(i), 4);

		d2E_dJ2 <<
			aa, ab, ac, ad,
			ab, bb, bc, bd,
			ac, bc, cc, cd,
			ad, bd, cd, dd;

		Hessian[i] = Area(i)*dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];

		int index2 = i * 21;
		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[index2++] = Hessian[i](a, b);
			}
		}
	}
}

bool symmetric_dirichlet::update_variables(const VectorXd& X)
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
		dirichlet(i) = a(i)*a(i) + b(i)*b(i) + c(i)*c(i) + d(i)*d(i);
		detJ(i) = a(i) * d(i) - b(i) * c(i);
	}
	return ((detJ.array() < 0).any());
}