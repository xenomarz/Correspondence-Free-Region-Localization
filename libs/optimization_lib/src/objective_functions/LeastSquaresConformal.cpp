#include <objective_functions/LeastSquaresConformal.h>

LeastSquaresConformal::LeastSquaresConformal()
{
	name = "Least Squares Conformal";
}

void LeastSquaresConformal::init()
{
	TriangleMeshObjectiveFunction::init();

	//prepare hessian and dJ/dX
	MatrixXd d2E_dJ2(4, 4);
	d2E_dJ2 <<
		2 , 0, 0, -2,
		0 , 2, 2, 0 ,
		0 , 2, 2, 0 ,
		-2, 0, 0, 2;
	for (int i = 0; i < F.rows(); i++) {
		Hessian[i] = Area(i)*dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
	}
}

double LeastSquaresConformal::value(const bool update)
{
	VectorXd E = (a-d).cwiseAbs2() + (b+c).cwiseAbs2();
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
		Vector4d dE_dJ(
			a(fi) - d(fi), 
			b(fi) + c(fi), 
			b(fi) + c(fi), 
			d(fi) - a(fi)
		);
		dE_dJ *= 2;
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