#include <objective_functions/LeastSquaresConformal.h>

LeastSquaresConformal::LeastSquaresConformal()
{
	name = "Least Squares Conformal";
	w = 0;
}

void LeastSquaresConformal::init()
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

	//prepare hessian and dJ/dX
	MatrixXd d2E_dJ2(4, 4);
	d2E_dJ2 <<
		4, 0, 0, 0,
		0, 2, 2, 0,
		0, 2, 2, 0,
		0, 0, 0, 4;
	for (int i = 0; i < F.rows(); i++) {
		MatrixXd Dx = D1d.col(i).transpose();
		MatrixXd Dy = D2d.col(i).transpose();
		MatrixXd zero = VectorXd::Zero(3).transpose();
		dJ_dX[i] << 
			Dx	, zero	,
			zero, Dx	,
			Dy	, zero	,
			zero, Dy;
	
		Hessian[i] = Area(i)*dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
	}
	
	init_hessian();
}

void LeastSquaresConformal::updateX(const VectorXd& X)
{
	bool inversions_exist = update_variables(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double LeastSquaresConformal::value(bool update)
{
	VectorXd E = 2*(d.cwiseAbs2()) + (b+c).cwiseAbs2() + 2 * (a.cwiseAbs2());
	double value = (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = value;
	}
	
	return value;
}

void LeastSquaresConformal::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Vector4d dE_dJ(4 * a(fi), 2 * b(fi) + 2 * c(fi), 2 * b(fi) + 2 * c(fi), 4 * d(fi));
		grad.row(fi) = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();

		//Update the gradient of the x-axis
		g(F(fi, 0)) += grad(fi,0);
		g(F(fi, 1)) += grad(fi, 1);
		g(F(fi, 2)) += grad(fi, 2);
		//Update the gradient of the y-axis
		g(F(fi, 0) + V.rows()) += grad(fi, 3);
		g(F(fi, 1) + V.rows()) += grad(fi, 4);
		g(F(fi, 2) + V.rows()) += grad(fi, 5);
	}
	gradient_norm = g.norm();
}

void LeastSquaresConformal::hessian()
{
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
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