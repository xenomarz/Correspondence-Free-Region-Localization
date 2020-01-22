#include "objective_functions/LagrangianLscmStArea.h"

LagrangianLscmStArea::LagrangianLscmStArea()
{
	name = "LagrangianLscmStArea";
	w = 1.0f;
}

double LagrangianLscmStArea::objectiveValue(const bool update) {
	Eigen::VectorXd LSCM = (a - d).cwiseAbs2() + (b + c).cwiseAbs2();
	double objVal = (Area.asDiagonal() * LSCM).sum();
	if (update) {
		objective_value = objVal;
	}
	return objVal;
}

Eigen::VectorXd LagrangianLscmStArea::constrainedValue(const bool update) {
	Eigen::VectorXd areaE = detJ - Eigen::VectorXd::Ones(F.rows());
	areaE = Area.asDiagonal() * areaE;
	if (update) {
		Efi = areaE.cwiseAbs2();
		constraint_value = areaE.sum();
	}
	return areaE;
}

double LagrangianLscmStArea::lagrangianValue(const bool update)
{
	// f(x) - objective function value 
	// c(x) - constraint function vector 
	// lambda - lagrange multipliers 
	// Lagrangian = f(x) - lambda * c(x)
	double lagrangian = objectiveValue(update) - lambda.cwiseProduct(constrainedValue(update)).sum();
	if (update) {
		energy_value = lagrangian;
	}
	return lagrangian;
}

double LagrangianLscmStArea::value(const bool update) {
	return lagrangianValue(update);
}

double LagrangianLscmStArea::AugmentedValue(const bool update)
{
	double augmented = lagrangianValue(update) + 
		(augmented_value_parameter / 2) * constrainedValue(update).cwiseAbs2().sum();

	return augmented;
}

void LagrangianLscmStArea::gradient(Eigen::VectorXd& g, const bool update)
{
	g.conservativeResize(V.rows() * 2 + F.rows());
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ(
			2 * a(fi) - 2 * d(fi) - lambda(fi) * d(fi),
			2 * b(fi) + 2 * c(fi) + lambda(fi) * c(fi),
			2 * b(fi) + 2 * c(fi) + lambda(fi) * b(fi),
			2 * d(fi) - 2 * a(fi) - lambda(fi) * a(fi)
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
		g(fi + 2 * V.rows()) += Area(fi)*(1 - detJ(fi));
	}
	if (update) {
		gradient_norm = g.norm();
		objective_gradient_norm = g.head(2 * V.rows()).norm();
		constraint_gradient_norm = g.tail(F.rows()).norm();
	}
}

void LagrangianLscmStArea::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		Eigen::MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			2			, 0				, 0				, -lambda(i) -2,
			0			, 2				, 2+lambda(i)	, 0			,
			0			, 2 + lambda(i)	, 2				, 0			,
			-lambda(i)-2	, 0				, 0				, 2;

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
		Eigen::Vector4d dE_dJ(
			-d(i),
			c(i),
			b(i),
			-a(i)
		);
		Eigen::VectorXd hess = Area(i)*(dE_dJ.transpose() * dJ_dX[i]).transpose();
		SS[index2++] = hess[0];
		SS[index2++] = hess[1];
		SS[index2++] = hess[2];
		SS[index2++] = hess[3];
		SS[index2++] = hess[4];
		SS[index2++] = hess[5];	
	}
}