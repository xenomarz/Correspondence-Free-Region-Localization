#include <objective_functions/objective_area_preserving.h>

ObjectiveAreaPreserving::ObjectiveAreaPreserving()
{
	name = "Objective: Area Preserving";
}

void ObjectiveAreaPreserving::init()
{
	if (V.size() == 0 || F.size() == 0)
		throw "DistortionAreaPreserving must define members V,F before init()!";
	
	a.resize(F.rows());
	b.resize(F.rows());
	c.resize(F.rows());
	d.resize(F.rows());
	
	//Parameterization J mats resize
	detJ.resize(F.rows());
	grad.resize(F.rows(), 6);
	Hessian.resize(F.rows());

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	MatrixX3d D1cols, D2cols, V3d;

	V3d.resize(V.rows(), 3);
	V3d.leftCols(2) = V;
	V3d.col(2).setZero();
	Utils::computeSurfaceGradientPerFace(V3d, F, D1cols, D2cols);
	D1d = D1cols.transpose();
	D2d = D2cols.transpose();

	prepare_hessian();
	w = 1;
}

void ObjectiveAreaPreserving::updateX(const VectorXd& X)
{
	bool inversions_exist = updateJ(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

double ObjectiveAreaPreserving::value()
{
	// E = (det(J) - 1)^2
	MatrixXd ones(detJ.rows(), 1);
	ones.setConstant(1);
	Efi = (detJ - ones).cwiseAbs2();

	return 0.5*(Area.asDiagonal()*Efi).sum();
}

void ObjectiveAreaPreserving::gradient(VectorXd& g)
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
}

void ObjectiveAreaPreserving::hessian()
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
	for (int i = 0; i < F.rows(); i++)
	{
		//adding epsilon to the diagonal to prevent solver's errors
		int base = 21 * i;
		SS[base] += 1e-6;
		SS[base + 2] += 1e-6;
		SS[base + 5] += 1e-6;
		SS[base + 9] += 1e-6;
		SS[base + 14] += 1e-6;
		SS[base + 20] += 1e-6;
	}
}

bool ObjectiveAreaPreserving::updateJ(const VectorXd& X)
{
	Eigen::Map<const MatrixX2d> x(X.data(), X.size() / 2, 2);
	// 	a = D1*U;
	// 	b = D2*U;
	// 	c = D1*V;
	// 	d = D2*V;
	for (int i = 0; i < F.rows(); i++)
	{
		// For each face Fi = (p0,p1,p2)
		//		Xi = [ p0.x , p1.x , p2.x ]
		//		Yi = [ p0.y , p1.y , p2.y ]
		Vector3d Xi, Yi;
		Xi << x(F(i, 0), 0), x(F(i, 1), 0), x(F(i, 2), 0);
		Yi << x(F(i, 0), 1), x(F(i, 1), 1), x(F(i, 2), 1);
		Matrix<double, 1, 3> Xi_T = Xi.transpose();
		Matrix<double, 1, 3> Yi_T = Yi.transpose();
		Vector3d Dx = D1d.col(i);
		Vector3d Dy = D2d.col(i);
		Matrix<double, 1, 3> Dx_T = Dx.transpose();
		Matrix<double, 1, 3> Dy_T = Dy.transpose();

		//prepare jacobian		
		a(i) = Dx_T * Xi;
		b(i) = Dy_T * Xi;
		c(i) = Dx_T * Yi;
		d(i) = Dy_T * Yi;

		//prepare gradient
		double detj_1 = (a(i) * d(i) - b(i) * c(i)) - 1;
		Vector3d dx = (Dx * Yi_T * Dy) - (Dy * Yi_T * Dx);
		dx *= detj_1;
		Vector3d dy = (Dy * Xi_T * Dx) - (Dx * Xi_T * Dy);
		dy *= detj_1;

		grad.row(i) << dx(0), dx(1), dx(2), dy(0), dy(1), dy(2);



		//prepare hessian
		double DyT_Yi = Dy_T * Yi;
		double YiT_Dy = Yi_T * Dy;
		double YiT_Dx = Yi_T * Dx;
		double DxT_Yi = Dx_T * Yi;
		double DyT_Xi = Dy_T * Xi;
		double DxT_Xi = Dx_T * Xi;
		double XiT_Dy = Xi_T * Dy;
		double XiT_Dx = Xi_T * Dx;

		Matrix<double, 3, 3> dxx =
			DyT_Yi * Dx * YiT_Dy*Dx_T
			- DyT_Yi * Dy * YiT_Dx*Dx_T
			- DxT_Yi * Dx * YiT_Dy*Dy_T
			+ DxT_Yi * Dy * YiT_Dx*Dy_T;

		Matrix<double, 3, 3> dyy =
			DyT_Xi * Dx * XiT_Dy*Dx_T
			- DyT_Xi * Dy * XiT_Dx*Dx_T
			- DxT_Xi * Dx * XiT_Dy*Dy_T
			+ DxT_Xi * Dy * XiT_Dx*Dy_T;

		Matrix<double, 3, 3> dxy =	//not sure!
			DyT_Yi * Dy * XiT_Dx*Dx_T
			+ DxT_Xi * DyT_Yi *Dy*Dx_T
			- DyT_Yi * Dx * XiT_Dy*Dx_T
			- DxT_Xi * DyT_Yi *Dx*Dy_T
			- DxT_Yi * Dy * XiT_Dx*Dy_T
			- DyT_Xi * DxT_Yi *Dy*Dx_T
			+ DxT_Yi * Dx * XiT_Dy*Dy_T
			+ DyT_Xi * DxT_Yi *Dx*Dy_T
			- Dy * Dx_T
			+ Dx * Dy_T;
		
		Matrix<double, 3, 3> dyx = dxy.transpose();
								
		Hessian[i] << dxx(0, 0), dxx(0, 1), dxx(0, 2), dxy(0, 0), dxy(0, 1), dxy(0, 2),
			dxx(1, 0), dxx(1, 1), dxx(1, 2), dxy(1, 0), dxy(1, 1), dxy(1, 2),
			dxx(2, 0), dxx(2, 1), dxx(2, 2), dxy(2, 0), dxy(2, 1), dxy(2, 2),

			dyx(0, 0), dyx(0, 1), dyx(0, 2), dyy(0, 0), dyy(0, 1), dyy(0, 2),
			dyx(1, 0), dyx(1, 1), dyx(1, 2), dyy(1, 0), dyy(1, 1), dyy(1, 2),
			dyx(2, 0), dyx(2, 1), dyx(2, 2), dyy(2, 0), dyy(2, 1), dyy(2, 2);

	}
	detJ = a.cwiseProduct(d) - b.cwiseProduct(c);
	
	return ((detJ.array() < 0).any());
}

//Here we build our hessian matrix with zeros
void ObjectiveAreaPreserving::prepare_hessian()
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
		PushPair(F(i, 0), F(i, 0));

		// Second column
		PushPair(F(i, 0), F(i, 1));
		PushPair(F(i, 1), F(i, 1));

		// Third column
		PushPair(F(i, 0), F(i, 2));
		PushPair(F(i, 1), F(i, 2));
		PushPair(F(i, 2), F(i, 2));

		// Fourth column
		PushPair(F(i, 0), F(i, 0) + V.rows());
		PushPair(F(i, 1), F(i, 0) + V.rows());
		PushPair(F(i, 2), F(i, 0) + V.rows());
		PushPair(F(i, 0) + V.rows(), F(i, 0) + V.rows());

		// Fifth column
		PushPair(F(i, 0), F(i, 1) + V.rows());
		PushPair(F(i, 1), F(i, 1) + V.rows());
		PushPair(F(i, 2), F(i, 1) + V.rows());
		PushPair(F(i, 0) + V.rows(), F(i, 1) + V.rows());
		PushPair(F(i, 1) + V.rows(), F(i, 1) + V.rows());

		// Sixth column
		PushPair(F(i, 0), F(i, 2) + V.rows());
		PushPair(F(i, 1), F(i, 2) + V.rows());
		PushPair(F(i, 2), F(i, 2) + V.rows());
		PushPair(F(i, 0) + V.rows(), F(i, 2) + V.rows());
		PushPair(F(i, 1) + V.rows(), F(i, 2) + V.rows());
		PushPair(F(i, 2) + V.rows(), F(i, 2) + V.rows());
	}
	SS = vector<double>(II.size(), 0.);
}