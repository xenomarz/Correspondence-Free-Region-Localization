#include <utils/utils.h>
#include <objective_functions/symmetric_dirichlet.h>
#include <limits>
#include <igl/doublearea.h>

SymmetricDirichlet::SymmetricDirichlet(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider)
	: ObjectiveFunction(objective_function_data_provider, "Symmetric Dirichlet")
{
	Initialize();
}

SymmetricDirichlet::~SymmetricDirichlet()
{

}

void SymmetricDirichlet::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.clear();
	jj.clear();

	auto PushPair = [&](int i, int j) {
		ii.push_back(i);
		jj.push_back(j);
	};

	auto nfs = objective_function_data_provider_->GetImageFaces().rows();
	auto nvs = objective_function_data_provider_->GetImageVerticesCount();
	for (int i = 0; i < nfs; ++i)
	{
		// for every face there is a 6x6 local hessian
		// we only need the 21 values contained in the upper
		// diagonal. they are access and also put into the
		// big hessian in column order.

		// base indices
		int uhbr = 3 * i;		//upper_half_base_row
		int lhbr = 3 * i + static_cast<int>(nvs); // lower_half_base_row 
		int lhbc = 3 * i;		// left_half_base_col 
		int rhbc = 3 * i + static_cast<int>(nvs); // right_half_base_col 

		// first column
		PushPair(uhbr, lhbc);

		// second column
		PushPair(uhbr, lhbc + 1);
		PushPair(uhbr + 1, lhbc + 1);

		// third column
		PushPair(uhbr, lhbc + 2);
		PushPair(uhbr + 1, lhbc + 2);
		PushPair(uhbr + 2, lhbc + 2);

		// fourth column
		PushPair(uhbr, rhbc);
		PushPair(uhbr + 1, rhbc);
		PushPair(uhbr + 2, rhbc);
		PushPair(lhbr, rhbc);

		// fifth column
		PushPair(uhbr, rhbc + 1);
		PushPair(uhbr + 1, rhbc + 1);
		PushPair(uhbr + 2, rhbc + 1);
		PushPair(lhbr, rhbc + 1);
		PushPair(lhbr + 1, rhbc + 1);

		// sixth column
		PushPair(uhbr, rhbc + 2);
		PushPair(uhbr + 1, rhbc + 2);
		PushPair(uhbr + 2, rhbc + 2);
		PushPair(lhbr, rhbc + 2);
		PushPair(lhbr + 1, rhbc + 2);
		PushPair(lhbr + 2, rhbc + 2);
	}

	ss = std::vector<double>(ii.size(), 0.);
}

void SymmetricDirichlet::CalculateValue(const Eigen::VectorXd& x, double& f, Eigen::VectorXd& f_per_vertex)
{
	bool inversions_exist = updateJ(X);

	// E = ||J||^2 + ||J^-1||^2 = ||J||^2 + ||J||^2 / det(J)^2
	Eigen::VectorXd dirichlet = a.cwiseAbs2() + b.cwiseAbs2() + c.cwiseAbs2() + d.cwiseAbs2();
	Eigen::VectorXd invDirichlet = dirichlet.cwiseQuotient(detJuv.cwiseAbs2());
	Efi = dirichlet + invDirichlet;

	f = 0.5 * (Area.asDiagonal() * Efi).sum();
}

void SymmetricDirichlet::CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g)
{
	bool inversions_exist = updateJ(X);
	UpdateSSVDFunction();

	Eigen::MatrixX2d invs = s.cwiseInverse();

	g.conservativeResize(X.size());
	g.setZero();
	ComputeDenseSSVDDerivatives();

	for (int fi = 0; fi < numF; ++fi)
	{
		double gS = s(fi, 0) - pow(invs(fi, 0), 3);
		double gs = s(fi, 1) - pow(invs(fi, 1), 3);

		if (bound > 0)
		{
			gS += gS / (bound - Efi(fi));
			gs += gs / (bound - Efi(fi));
		}

		Eigen::Matrix<double, 6, 1> Dsdi0 = Dsd[0].col(fi);
		Eigen::Matrix<double, 6, 1> Dsdi1 = Dsd[1].col(fi);
		Eigen::Matrix<double, 6, 1> gi = Area(fi) * (Dsdi0 * gS + Dsdi1 * gs);

		for (int vi = 0; vi < 6; ++vi)
		{
			g(Fuv(vi, fi)) += gi(vi);
		}
	}
}

void SymmetricDirichlet::CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss)
{
	auto lambda1 = [](double a) {return a - 1.0 / (a * a * a); };

	// gradient of outer function in composition
	Eigen::VectorXd gradfS = s.col(0).unaryExpr(lambda1);
	Eigen::VectorXd gradfs = s.col(1).unaryExpr(lambda1);
	auto lambda2 = [](double a) {return 1 + 3 / (a * a * a * a); };

	// hessian of outer function in composition (diagonal)
	Eigen::VectorXd HS = s.col(0).unaryExpr(lambda2);
	Eigen::VectorXd Hs = s.col(1).unaryExpr(lambda2);

	// similarity alpha
	Eigen::VectorXd aY = 0.5 * (a + d);
	Eigen::VectorXd bY = 0.5 * (c - b);

	// anti similarity beta
	Eigen::VectorXd cY = 0.5 * (a - d);
	Eigen::VectorXd dY = 0.5 * (b + c);

	#pragma omp parallel for num_threads(24)
	for (int i = 0; i < numF; ++i) {
		//vectors of size 6
		//svd derivatives
		Eigen::Matrix<double, 6, 1> dSi = Dsd[0].col(i);
		Eigen::Matrix<double, 6, 1> dsi = Dsd[1].col(i);
		//cones constant coefficients (cone = |Ax|, A is a coefficient)
		Eigen::Matrix<double, 6, 1> a1i = a1d.col(i);
		Eigen::Matrix<double, 6, 1> a2i = a2d.col(i);
		Eigen::Matrix<double, 6, 1> b1i = b1d.col(i);
		Eigen::Matrix<double, 6, 1> b2i = b2d.col(i);
		Hi[i] = Area(i) * ComputeConvexConcaveFaceHessian(
			a1i,
			a2i,
			b1i,
			b2i,
			aY(i),
			bY(i),
			cY(i),
			dY(i),
			dSi,
			dsi,
			gradfS(i),
			gradfs(i),
			HS(i),
			Hs(i));

		int index2 = i * 21;
		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				ss[index2++] = Hi[i](a, b);
			}
		}
	}

	std::size_t nf = F.rows();
	for (std::size_t i = 0; i < nf; i++)
	{
		std::size_t base = 21 * i;
		ss[base] += 1e-6;
		ss[base + 2] += 1e-6;
		ss[base + 5] += 1e-6;
		ss[base + 9] += 1e-6;
		ss[base + 14] += 1e-6;
		ss[base + 20] += 1e-6;
	}
}

void SymmetricDirichlet::PreUpdate(const Eigen::VectorXd& x)
{
	X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
}

bool SymmetricDirichlet::updateJ(const Eigen::MatrixX2d& x)
{
	Eigen::Map<const Eigen::Matrix3Xd> X1(x.data(), 3, F.rows());
	Eigen::Map<const Eigen::Matrix3Xd> X2(x.col(1).data(), 3, F.rows());
	a = D1d.cwiseProduct(X1).colwise().sum();
	b = D2d.cwiseProduct(X1).colwise().sum();
	c = D1d.cwiseProduct(X2).colwise().sum();
	d = D2d.cwiseProduct(X2).colwise().sum();
	detJuv = a.cwiseProduct(d) - b.cwiseProduct(c);

	return ((detJuv.array() < 0).any());
}

void SymmetricDirichlet::UpdateSSVDFunction()
{
	#pragma omp parallel for num_threads(24)
	for (int i = 0; i < a.size(); i++)
	{
		Eigen::Matrix2d A;
		Eigen::Matrix2d U, S, V;
		A << a[i], b[i], c[i], d[i];
		Utils::SSVD2x2(A, U, S, V);
		u.row(i) << U(0), U(1), U(2), U(3);
		v.row(i) << V(0), V(1), V(2), V(3);
		s.row(i) << S(0), S(3);
	}
}

void SymmetricDirichlet::ComputeDenseSSVDDerivatives()
{
	// different columns belong to different faces
	Eigen::MatrixXd B(D1d*v.col(0).asDiagonal() + D2d*v.col(1).asDiagonal());
	Eigen::MatrixXd C(D1d*v.col(2).asDiagonal() + D2d*v.col(3).asDiagonal());

	Eigen::MatrixXd t1 = B* u.col(0).asDiagonal();
	Eigen::MatrixXd t2 = B* u.col(1).asDiagonal();
	Dsd[0].topRows(t1.rows()) = t1;
	Dsd[0].bottomRows(t1.rows()) = t2;
	t1 = C*u.col(2).asDiagonal();
	t2 = C*u.col(3).asDiagonal();
	Dsd[1].topRows(t1.rows()) = t1;
	Dsd[1].bottomRows(t1.rows()) = t2;
}

inline Eigen::Matrix<double, 6, 6> SymmetricDirichlet::ComputeFaceConeHessian(
	const Eigen::Matrix<double, 6, 1>& A1,
	const Eigen::Matrix<double, 6, 1>& A2,
	double a1x,
	double a2x)
{
	double f2 = a1x*a1x + a2x*a2x;
	double invf = 1.0/sqrt(f2);
	double invf3 = invf*invf*invf;

	Eigen::Matrix<double, 6, 6> A1A1t = A1*A1.transpose();
	Eigen::Matrix<double, 6, 6> A2A2t = A2*A2.transpose();
	Eigen::Matrix<double, 6, 6> A1A2t = A1*A2.transpose();
	Eigen::Matrix<double, 6, 6> A2A1t = A1A2t.transpose();

	double a2 = a1x*a1x; 
	double b2 = a2x*a2x; 
	double ab = a1x*a2x; 

	return  (invf - invf3*a2) * A1A1t + (invf - invf3*b2) * A2A2t - invf3 * ab*(A1A2t + A2A1t);
}

inline Eigen::Matrix<double, 6, 6> SymmetricDirichlet::ComputeConvexConcaveFaceHessian(
	const Eigen::Matrix<double, 6, 1>& a1,
	const Eigen::Matrix<double, 6, 1>& a2,
	const Eigen::Matrix<double, 6, 1>& b1,
	const Eigen::Matrix<double, 6, 1>& b2,
	double aY,
	double bY,
	double cY,
	double dY,
	const Eigen::Matrix<double, 6, 1>& dSi,
	const Eigen::Matrix<double, 6, 1>& dsi,
	double gradfS,
	double gradfs,
	double HS,
	double Hs)
{
	// no multiplying by area in this function

	// generalized gauss newton
	Eigen::Matrix<double, 6, 6> H = HS*dSi*dSi.transpose() + Hs*dsi*dsi.transpose();
	double walpha = gradfS + gradfs;
	if (walpha > 0)
	{
		H += walpha * ComputeFaceConeHessian(a1, a2, aY, bY);
	}

	double wbeta = gradfS - gradfs;
	if (wbeta > 1e-7)
	{
		H += wbeta * ComputeFaceConeHessian(b1, b2, cY, dY);
	}

	return H;
}

void SymmetricDirichlet::PreInitialize()
{
	ObjectiveFunction::PreInitialize();

	auto F = objective_function_data_provider_->GetDomainFaces();
	auto V = objective_function_data_provider_->GetDomainVertices();

	auto D1 = objective_function_data_provider_->GetD1();
	auto D2 = objective_function_data_provider_->GetD2();

	auto Fs = objective_function_data_provider_->GetImageFaces();
	this->F = objective_function_data_provider_->GetImageFaces();

	numF = Fs.rows();
	numV = objective_function_data_provider_->GetImageVerticesCount();

	Fuv.resize(6, numF);
	Fuv.topRows(3) = Fs.transpose();
	Fuv.bottomRows(3) = Fuv.topRows(3) + Eigen::MatrixXi::Constant(3, numF, static_cast<int>(numV));

	a.resize(numF);
	b.resize(numF);
	c.resize(numF);
	d.resize(numF);
	s.resize(numF, 2);
	v.resize(numF, 4);
	u.resize(numF, 4);

	Dsd[0].resize(6, numF);
	Dsd[1].resize(6, numF);

	//Parameterization J mats resize
	detJuv.resize(numF);
	invdetJuv.resize(numF);
	DdetJuv_DUV.resize(static_cast<int>(numF), static_cast<int>(numV * 2));

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	D1d = D1.transpose();
	D2d = D2.transpose();

	//columns belong to different faces
	a1d.resize(6, numF);
	a2d.resize(6, numF);
	b1d.resize(6, numF);
	b2d.resize(6, numF);

	a1d.topRows(3) = 0.5 * D1d;
	a1d.bottomRows(3) = 0.5 * D2d;

	a2d.topRows(3) = -0.5 * D2d;
	a2d.bottomRows(3) = 0.5 * D1d;

	b1d.topRows(3) = 0.5 * D1d;
	b1d.bottomRows(3) = -0.5 * D2d;

	b2d.topRows(3) = 0.5 * D2d;
	b2d.bottomRows(3) = 0.5 * D1d;

	Hi.resize(numF);
}