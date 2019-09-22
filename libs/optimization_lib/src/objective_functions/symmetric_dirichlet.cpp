#include <objective_functions/symmetric_dirichlet.h>

SymmetricDirichlet::SymmetricDirichlet()
{
	name = "symmetric dirichlet";
	w = 0.1;
}

void SymmetricDirichlet::init()
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

	prepare_hessian();
}

void SymmetricDirichlet::updateX(const VectorXd& X)
{
	bool inversions_exist = updateJ(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double SymmetricDirichlet::value(bool update)
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

void SymmetricDirichlet::gradient(VectorXd& g)
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

void SymmetricDirichlet::hessian()
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

bool SymmetricDirichlet::updateJ(const VectorXd& X)
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
		

		//prepare gradient
		Vector4d dE_dJ;
		dE_dJ <<
			a(i) + (a(i) / pow(detJ(i), 2)) - ((d(i)*dirichlet(i)) / pow(detJ(i), 3)),
			b(i) + (b(i) / pow(detJ(i), 2)) + ((c(i)*dirichlet(i)) / pow(detJ(i), 3)),
			c(i) + (c(i) / pow(detJ(i), 2)) + ((b(i)*dirichlet(i)) / pow(detJ(i), 3)),
			d(i) + (d(i) / pow(detJ(i), 2)) - ((a(i)*dirichlet(i)) / pow(detJ(i), 3));

		grad.row(i) = (dE_dJ.transpose() * dJ_dX[i]).transpose();

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

		double ad = (3*a(i)*d(i)*dirichlet(i))
			-((2*pow(a(i),2)+ 2*pow(d(i), 2)+dirichlet(i))*detJ(i));
		ad /= pow(detJ(i), 4);

		double bc = (3 * b(i)*c(i)*dirichlet(i))
			+((2 * pow(b(i), 2) + 2 * pow(c(i), 2) + dirichlet(i))*detJ(i));
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


		Hessian[i] = dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
	}
	
	
	return ((detJ.array() < 0).any());
}

void SymmetricDirichlet::prepare_hessian()
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