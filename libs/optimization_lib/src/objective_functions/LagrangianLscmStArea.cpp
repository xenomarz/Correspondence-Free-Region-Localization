#include <objective_functions/LagrangianLscmStArea.h>

LagrangianLscmStArea::LagrangianLscmStArea()
{
	name = "LagrangianLscmStArea";
}

double LagrangianLscmStArea::value(bool update)
{
	// L = LSCM + lambda * area
	
	VectorXd LSCM = 2 * d.cwiseAbs2() + (b + c).cwiseAbs2() + 2 * a.cwiseAbs2();
	VectorXd areaE = detJ - VectorXd::Ones(F.rows());
	
	VectorXd E = LSCM + lambda.cwiseProduct(areaE);
	double value = (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = value;
		objective_value = (Area.asDiagonal() * LSCM).sum();
		constraint_value = (Area.asDiagonal() * areaE).sum();
	}
	
	return value;
}

double LagrangianLscmStArea::AugmentedValue(const bool update)
{
	// Augmented_L = L + k * ||LSCM||^2
	VectorXd areaE = detJ - VectorXd::Ones(F.rows());
	//I am not sure of multiplying areaE by Area!!!
	double augmented_part = (Area.asDiagonal() * areaE.cwiseAbs2()).sum();
	
	return value(update) + augmented_value_parameter * augmented_part;
}

void LagrangianLscmStArea::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2 + F.rows());
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Vector4d dE_dJ(
			4 * a(fi) + lambda(fi) * d(fi), 
			2 * b(fi) + 2 * c(fi) - lambda(fi) * c(fi),
			2 * b(fi) + 2 * c(fi) - lambda(fi) * b(fi),
			4 * d(fi) + lambda(fi) * a(fi)
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
		g(fi + 2 * V.rows()) += Area(fi)*(detJ(fi) - 1);
	}
	gradient_norm = g.norm();
	objective_gradient_norm = g.head(2 * V.rows()).norm();
	constraint_gradient_norm = g.tail(F.rows()).norm();
}

void LagrangianLscmStArea::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			4			, 0				, 0				, lambda(i),
			0			, 2				, 2-lambda(i)	, 0			,
			0			, 2 - lambda(i)	, 2				, 0			,
			lambda(i)	, 0				, 0				, 4;

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
			d(i),
			-c(i),
			-b(i),
			a(i)
		);
		VectorXd hess = Area(i)*(dE_dJ.transpose() * dJ_dX[i]).transpose();
		SS[index2++] = hess[0];
		SS[index2++] = hess[1];
		SS[index2++] = hess[2];
		SS[index2++] = hess[3];
		SS[index2++] = hess[4];
		SS[index2++] = hess[5];	
	}
}