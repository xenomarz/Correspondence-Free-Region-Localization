#include <objective_functions/AreaDistortion.h>

AreaDistortion::AreaDistortion()
{
	name = "Area Preserving";
}

double AreaDistortion::value(const bool update)
{
	// E = 0.5(det(J) - 1)^2
	Eigen::VectorXd E = (detJ - Eigen::VectorXd::Ones(F.rows())).cwiseAbs2();
	double Evalue = 0.5 * (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = Evalue;
	}
	
	return Evalue;
}

void AreaDistortion::gradient(Eigen::VectorXd& g, const bool update)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ(d(fi), -c(fi), -b(fi), a(fi));
		dE_dJ *= (detJ(fi) - 1);
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

void AreaDistortion::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int i = 0; i < F.rows(); ++i) {
		double detj_1 = (a(i) * d(i) - b(i) * c(i)) - 1;

		//prepare hessian
		Eigen::MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			d(i)*d(i)			, -c(i)*d(i)			, -b(i)*d(i)		, a(i)*d(i) + detj_1,
			-c(i)*d(i)			, c(i)*c(i)				, b(i)*c(i) - detj_1, -c(i)*a(i),
			-b(i)*d(i)			, b(i)*c(i) - detj_1	, b(i)*b(i)			, -b(i)*a(i),
			a(i)*d(i) + detj_1	, -a(i)*c(i)			, -a(i)*b(i)		, a(i)*a(i);

		Hessian[i] = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];

		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[index2++] = Hessian[i](a, b);
			}
		}
	}
}