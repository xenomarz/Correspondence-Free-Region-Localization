#include <objective_functions/LagrangianAreaStLscm.h>

LagrangianAreaStLscm::LagrangianAreaStLscm()
{
	name = "LagrangianAreaStLscm";
}

double LagrangianAreaStLscm::value(const bool update)
{
	// L = area + lambda * LSCM
	VectorXd LSCM = (a - d).cwiseAbs2() + (b + c).cwiseAbs2();
	VectorXd areaE = 0.5 * ((detJ - VectorXd::Ones(F.rows())).cwiseAbs2());
	
	VectorXd E = areaE + lambda.cwiseProduct(LSCM);
	double value = (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = value;
		objective_value = (Area.asDiagonal() * areaE).sum();
		constraint_value = (Area.asDiagonal() * LSCM).sum();
	}
	
	return value;
}

double LagrangianAreaStLscm::AugmentedValue(const bool update)
{
	// Augmented_L = L + k * ||LSCM||^2
	VectorXd LSCM = (a - d).cwiseAbs2() + (b + c).cwiseAbs2();
	//I am not sure of multiplying areaE by Area!!!
	double augmented_part = (Area.asDiagonal() * LSCM.cwiseAbs2()).sum();
	
	return value(update) + augmented_value_parameter * augmented_part;
}

void LagrangianAreaStLscm::gradient(VectorXd& g, const bool update)
{
	g.conservativeResize(V.rows() * 2 + F.rows());
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Vector4d dE_dJ(
			(detJ(fi) - 1)*d(fi) + lambda(fi)*(2*a(fi) - 2*d(fi)),
			(1 - detJ(fi))*c(fi) + lambda(fi)*(2 * b(fi) + 2 * c(fi)),
			(1 - detJ(fi))*b(fi) + lambda(fi)*(2 * b(fi) + 2 * c(fi)),
			(detJ(fi) - 1)*a(fi) + lambda(fi)*(2 * d(fi) - 2 * a(fi))
		);
		grad.row(fi) = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();
		
		
		//Update the gradient of the x-axis
		g(F(fi, 0)) += grad(fi, 0);
		g(F(fi, 1)) += grad(fi, 1);
		g(F(fi, 2)) += grad(fi, 2);
		//Update the gradient of the y-axis
		g(F(fi, 0) + V.rows()) += grad(fi, 3);
		g(F(fi, 1) + V.rows()) += grad(fi, 4);
		g(F(fi, 2) + V.rows()) += grad(fi, 5);
		//Update the gradient of lambda
		g(fi + 2 * V.rows()) += Area(fi)* (pow(a(fi) - d(fi), 2) + pow(b(fi) + c(fi), 2));
	}

	if (update) {
		gradient_norm = g.norm();
		objective_gradient_norm = g.head(2 * V.rows()).norm();
		constraint_gradient_norm = g.tail(F.rows()).norm();
	}
}

void LagrangianAreaStLscm::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			pow(d(i), 2) + 2 * lambda(i)					, -c(i)*d(i)									, -b(i)*d(i)									, 2*a(i)*d(i)-b(i)*c(i)-1-2* lambda(i)	,
			-c(i)*d(i)										, pow(c(i), 2) + 2 * lambda(i)					, -a(i)*d(i) + 2 * b(i)*c(i) + 1 + 2 * lambda(i), -a(i)*c(i)							,
			-b(i)*d(i)										, -a(i)*d(i) + 2 * b(i)*c(i) + 1 + 2 * lambda(i), pow(b(i), 2) + 2 * lambda(i)					, -a(i)*b(i)							,
			2 * a(i)*d(i) - b(i)*c(i) - 1 - 2 * lambda(i)	, -a(i)*c(i)									, -a(i)*b(i)									, pow(a(i), 2) + 2 * lambda(i);

		Hessian[i] = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];

		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[index2++] = Hessian[i](a, b);
			}
		}
	}
	
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		Vector4d dE_dJ(
			a(i) - d(i),
			b(i) + c(i),
			b(i) + c(i),
			d(i) - a(i)
		);
		dE_dJ *= 2;
		VectorXd hess = Area(i)*(dE_dJ.transpose() * dJ_dX[i]).transpose();
		SS[index2++] = hess[0];
		SS[index2++] = hess[1];
		SS[index2++] = hess[2];
		SS[index2++] = hess[3];
		SS[index2++] = hess[4];
		SS[index2++] = hess[5];	
	}
}