#include <objective_functions/AreaDistortion.h>

AreaDistortion::AreaDistortion()
{
	name = "Area Preserving";
	w = 1;
}

void AreaDistortion::init()
{
	TriangleMeshObjectiveFunction::init();
	grad.resize(F.rows(), 6);
	Hessian.resize(F.rows());
	init_hessian();
}

void AreaDistortion::updateX(const VectorXd& X)
{
	bool inversions_exist = update_variables(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double AreaDistortion::value(bool update)
{
	// E = 0.5(det(J) - 1)^2
	VectorXd E = (detJ - VectorXd::Ones(detJ.rows())).cwiseAbs2();
	double value = 0.5 * (Area.asDiagonal() * E).sum();
	
	if (update) {
		Efi = E;
		energy_value = value;
	}
	
	return value;
}

void AreaDistortion::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Vector4d dE_dJ(d(fi), -c(fi), -b(fi), a(fi));
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
	gradient_norm = g.norm();
}

void AreaDistortion::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int i = 0; i < F.rows(); ++i) {
		double detj_1 = (a(i) * d(i) - b(i) * c(i)) - 1;

		//prepare hessian
		MatrixXd d2E_dJ2(4, 4);
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

bool AreaDistortion::update_variables(const VectorXd& X)
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
	}
	detJ = a.cwiseProduct(d) - b.cwiseProduct(c);

	return ((detJ.array() < 0).any());
}

void AreaDistortion::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToHessian({ F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });
	SS = vector<double>(II.size(), 0.);
}