#include "objective_functions/SymmetricDirichlet.h"

SymmetricDirichlet::SymmetricDirichlet()
{
	name = "symmetric dirichlet";
}

void SymmetricDirichlet::init()
{
	TriangleMeshObjectiveFunction::init();
	dirichlet.resize(F.rows());
}

double SymmetricDirichlet::value(const bool update)
{
	Eigen::VectorXd invDirichlet = dirichlet.cwiseQuotient(detJ.cwiseAbs2());
	Eigen::VectorXd E = dirichlet + invDirichlet;
	double value = 0.5 * (Area.asDiagonal() * E).sum();
	if (update) {
		Efi = E;
		energy_value = value;
	}

	return value;
}

void SymmetricDirichlet::gradient(Eigen::VectorXd& g, const bool update)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ;
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
	if(update)
		gradient_norm = g.norm();
}

void SymmetricDirichlet::hessian()
{
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		Eigen::MatrixXd d2E_dJ2(4, 4);
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

bool SymmetricDirichlet::update_variables(const Eigen::VectorXd& X) {
	bool r = TriangleMeshObjectiveFunction::update_variables(X);
	dirichlet = a.cwiseProduct(a) + b.cwiseProduct(b) + c.cwiseProduct(c) + d.cwiseProduct(d);
	return r;
}