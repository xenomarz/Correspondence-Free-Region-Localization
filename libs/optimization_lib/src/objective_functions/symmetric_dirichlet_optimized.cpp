#include <objective_functions/symmetric_dirichlet_optimized.h>

SymmetricDirichletOptimized::SymmetricDirichletOptimized()
{
    name = "symmetric dirichlet optimized";
	w = 0;
}

void SymmetricDirichletOptimized::init()
{
	if (V.size() == 0 || F.size() == 0)
		throw name + " must define members V,F before init()!";
		
	a.resize(F.rows());
	b.resize(F.rows());
	c.resize(F.rows());
	d.resize(F.rows());
    alpha.resize(F.rows(), 2);
    beta.resize(F.rows(), 2);
	s.resize(F.rows(), 2);
	v.resize(F.rows(), 4);
	u.resize(F.rows(), 4);

	Dsd[0].resize(6, F.rows());
	Dsd[1].resize(6, F.rows());

	// Parameterization J mats resize
	detJ.resize(F.rows());

	MatrixX3d D1cols, D2cols;
	

	// Compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;
	
	Utils::computeSurfaceGradientPerFace(V, F, D1cols, D2cols);
	D1d=D1cols.transpose();
	D2d=D2cols.transpose();

	// Columns belong to different faces
	a1d.resize(6, F.rows());
	a2d.resize(6, F.rows());
	b1d.resize(6, F.rows());
	b2d.resize(6, F.rows());

	a1d.topRows(3) = 0.5*D1d;
	a1d.bottomRows(3) = 0.5*D2d;

	a2d.topRows(3) = -0.5*D2d;
	a2d.bottomRows(3) = 0.5*D1d;

	b1d.topRows(3) = 0.5*D1d;
	b1d.bottomRows(3) = -0.5*D2d;

	b2d.topRows(3) = 0.5*D2d;
	b2d.bottomRows(3) = 0.5*D1d;

	prepare_hessian();
}

void SymmetricDirichletOptimized::updateX(const VectorXd& X)
{
	bool inversions_exist = updateJ(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double SymmetricDirichletOptimized::value(bool update)
{
	// E = ||J||^2+||J^-1||^2 = ||J||^2+||J||^2/det(J)^2
	VectorXd dirichlet = a.cwiseAbs2() + b.cwiseAbs2() + c.cwiseAbs2() + d.cwiseAbs2();
	VectorXd invDirichlet = dirichlet.cwiseQuotient(detJ.cwiseAbs2());

	VectorXd E = dirichlet + invDirichlet;
	double value = 0.5 * (Area.asDiagonal() * E).sum();
	if (update) {
		Efi = E;
		energy_value = value;
	}
	
	return value;
}

void SymmetricDirichletOptimized::gradient(VectorXd& g)
{
    // Energy is h(S(x),s(x)), then grad_x h = grad_(S,s) h * [grad(S); grad(s)]
    MatrixX2d S(alpha);
    S.col(0) = alpha.rowwise().norm() + beta.rowwise().norm();
    S.col(1) = alpha.rowwise().norm() - beta.rowwise().norm();
	
	MatrixX2d invs = s.cwiseInverse();

	g.conservativeResize(V.rows() *2);
	g.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
        Vector2d dhdS(s(fi, 0) - pow(invs(fi, 0), 3), s(fi, 1) - pow(invs(fi, 1), 3));
        Vector6d dnormAlphadx = (a1d.col(fi)*alpha(fi, 0) + a2d.col(fi)*alpha(fi, 1))/alpha.row(fi).norm();
        Vector6d dnormBetadx = (b1d.col(fi)*beta(fi, 0) + b2d.col(fi)*beta(fi, 1))/(beta.row(fi).norm()+1e-6);
        Vector6d dSdx = dnormAlphadx + dnormBetadx;
        Vector6d dsdx = dnormAlphadx - dnormBetadx;
        Vector6d gi = Area(fi)*(dSdx * dhdS(0) + dsdx * dhdS(1));
		
		//Update the gradient of the x-axis
		g(F(fi,0)) += gi(0);
		g(F(fi,1)) += gi(1);
		g(F(fi,2)) += gi(2);
		//Update the gradient of the y-axis
		g(F(fi,0) + V.rows()) += gi(3);
		g(F(fi,1) + V.rows()) += gi(4);
		g(F(fi,2) + V.rows()) += gi(5);
	}
	gradient_norm = g.norm();
}

void SymmetricDirichletOptimized::hessian()
{
    UpdateSSVDFunction();
    ComputeDenseSSVDDerivatives();

	auto lambda1 = [](double a) {return a - 1.0 / (a*a*a); };
	//gradient of outer function in composition
	VectorXd gradfS = s.col(0).unaryExpr(lambda1);
	VectorXd gradfs = s.col(1).unaryExpr(lambda1);
	auto lambda2 = [](double a) {return 1 + 3 / (a*a*a*a); };
	//hessian of outer function in composition (diagonal)
	VectorXd HS = s.col(0).unaryExpr(lambda2);
	VectorXd Hs = s.col(1).unaryExpr(lambda2);
	//simliarity alpha
	VectorXd aY = 0.5*(a + d);
	VectorXd bY = 0.5*(c - b);
	//anti similarity beta
	VectorXd cY = 0.5*(a - d);
	VectorXd dY = 0.5*(b + c);
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
		//vectors of size 6
		//svd derivatives
		Vector6d dSi = Dsd[0].col(i);
		Vector6d dsi = Dsd[1].col(i);
		//cones constant coefficients (cone = |Ax|, A is a coefficient)
		Vector6d a1i = a1d.col(i);
		Vector6d a2i = a2d.col(i);
		Vector6d b1i = b1d.col(i);
		Vector6d b2i = b2d.col(i);
		Matrix<double, 6, 6> Hi = Area(i)*ComputeConvexConcaveFaceHessian(
			a1i, a2i, b1i, b2i,
			aY(i), bY(i), cY(i), dY(i),
			dSi, dsi,
			gradfS(i), gradfs(i),
			HS(i), Hs(i));

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

bool SymmetricDirichletOptimized::updateJ(const VectorXd& X)
{
	Map<const MatrixX2d> x(X.data(), X.size() / 2, 2);
	// 	a = D1*U;
	// 	b = D2*U;
	// 	c = D1*V;
	// 	d = D2*V;
	for (int i = 0; i < F.rows(); i++)
	{
		Vector3d X1i, X2i;
		X1i << x(F(i, 0), 0), x(F(i, 1), 0), x(F(i, 2), 0);
		X2i << x(F(i, 0), 1), x(F(i, 1), 1), x(F(i, 2), 1);
		a(i) = D1d.col(i).transpose()*X1i;
		b(i) = D2d.col(i).transpose()*X1i;
		c(i) = D1d.col(i).transpose()*X2i;
		d(i) = D2d.col(i).transpose()*X2i;
	}
	detJ = a.cwiseProduct(d) - b.cwiseProduct(c);
    alpha.col(0) = 0.5*(a + d);   alpha.col(1) = 0.5*(c - b);
    beta.col(0) = 0.5*(a - d);    beta.col(1) = 0.5*(c + b);
	
	return ((detJ.array() < 0).any());
};

void SymmetricDirichletOptimized::UpdateSSVDFunction()
{
	#pragma omp parallel for num_threads(24)
	for (int i = 0; i < a.size(); i++)
	{
		Matrix2d A;
		Matrix2d U, S, V;
		A << a[i], b[i], c[i], d[i];
		Utils::SSVD2x2(A, U, S, V);
		u.row(i) << U(0), U(1), U(2), U(3);
		v.row(i) << V(0), V(1), V(2), V(3);
		s.row(i) << S(0), S(3);
	}
}

void SymmetricDirichletOptimized::ComputeDenseSSVDDerivatives()
{
	// Different columns belong to diferent faces
	MatrixXd B(D1d*v.col(0).asDiagonal() + D2d*v.col(1).asDiagonal());
	MatrixXd C(D1d*v.col(2).asDiagonal() + D2d*v.col(3).asDiagonal());

	MatrixXd t1 = B* u.col(0).asDiagonal();
	MatrixXd t2 = B* u.col(1).asDiagonal();
	Dsd[0].topRows(t1.rows()) = t1;
	Dsd[0].bottomRows(t1.rows()) = t2;
	t1 = C*u.col(2).asDiagonal();
	t2 = C*u.col(3).asDiagonal();
	Dsd[1].topRows(t1.rows()) = t1;
	Dsd[1].bottomRows(t1.rows()) = t2;
}

inline Matrix6d SymmetricDirichletOptimized::ComputeFaceConeHessian(const Vector6d& A1, const Vector6d& A2, double a1x, double a2x)
{
	double f2 = a1x*a1x + a2x*a2x;
	double invf = 1.0/sqrt(f2);
	double invf3 = invf*invf*invf;

	Matrix6d A1A1t = A1*A1.transpose();
	Matrix6d A2A2t = A2*A2.transpose();
	Matrix6d A1A2t = A1*A2.transpose();
	Matrix6d A2A1t = A1A2t.transpose();

	double a2 = a1x*a1x; 
	double b2 = a2x*a2x; 
	double ab = a1x*a2x; 

	return  (invf - invf3*a2) * A1A1t + (invf - invf3*b2) * A2A2t - invf3 * ab*(A1A2t + A2A1t);
}

inline Matrix6d SymmetricDirichletOptimized::ComputeConvexConcaveFaceHessian(const Vector6d& a1, const Vector6d& a2, const Vector6d& b1, const Vector6d& b2, double aY, double bY, double cY, double dY, const Vector6d& dSi, const Vector6d& dsi, double gradfS, double gradfs, double HS, double Hs)
{
	// No multiplying by area in this function
	Matrix6d H = HS*dSi*dSi.transpose() + Hs*dsi*dsi.transpose(); //generalized gauss newton
	double walpha = gradfS + gradfs;
	if (walpha > 0)
		H += walpha*ComputeFaceConeHessian(a1, a2, aY, bY);

	double wbeta = gradfS - gradfs;
	if (wbeta > 1e-7)
		H += wbeta*ComputeFaceConeHessian(b1, b2, cY, dY);
	return H;
}

void SymmetricDirichletOptimized::prepare_hessian()
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