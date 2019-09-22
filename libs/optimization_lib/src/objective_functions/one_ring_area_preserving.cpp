#include <objective_functions/one_ring_area_preserving.h>

OneRingAreaPreserving::OneRingAreaPreserving()
{
	name = "One Ring Area Preserving";
	w = 0;
}

void OneRingAreaPreserving::init()
{
	if (V.size() == 0 || F.size() == 0)
		throw name + " must define members V,F before init()!";
	

	igl::vertex_triangle_adjacency(V, F, VF, VFi);

	a.resize(F.rows());
	b.resize(F.rows());
	c.resize(F.rows());
	d.resize(F.rows());
	
	//Parameterization J mats resize
	detJ.resize(F.rows());
	OneRingSum.resize(V.rows());
	grad.resize(V.rows());
	Hessian.resize(V.rows());
	dJ_dX.resize(V.rows());

	// compute init energy matrices
	igl::doublearea(V, F, Area);
	Area /= 2;

	MatrixX3d D1cols, D2cols;

	Utils::computeSurfaceGradientPerFace(V, F, D1cols, D2cols);
	D1d = D1cols.transpose();
	D2d = D2cols.transpose();

	//prepare dJ/dX
	for (int vi = 0; vi < VF.size(); vi++) {
		vector<int> OneRingFaces = VF[vi];
		vector<int> OneRingVertices = get_one_ring_vertices(OneRingFaces);

		int J_size = 4 * OneRingFaces.size();
		int X_size = 2 * OneRingVertices.size();

		dJ_dX[vi].resize(J_size, X_size);
		dJ_dX[vi].setZero();
		for (int i = 0; i < OneRingFaces.size(); i++) {
			int fi = OneRingFaces[i];
			int base_row = 4 * i;
			RowVectorXd Dx = D1d.col(fi).transpose();
			RowVectorXd Dy = D2d.col(fi).transpose();

			//Find the indexes of the face's vertices (p0,p1,p2) on the gradient vector
			int x0 = distance(OneRingVertices.begin(), find(OneRingVertices.begin(), OneRingVertices.end(), F(fi, 0)));
			int x1 = distance(OneRingVertices.begin(), find(OneRingVertices.begin(), OneRingVertices.end(), F(fi, 1)));
			int x2 = distance(OneRingVertices.begin(), find(OneRingVertices.begin(), OneRingVertices.end(), F(fi, 2)));
			int y0 = x0 + OneRingVertices.size();
			int y1 = x1 + OneRingVertices.size();
			int y2 = x2 + OneRingVertices.size();
			
			// update the gradient for: 
			// X cordinate of the first vertex 
			dJ_dX[vi](base_row + 0, x0) += Dx(0);
			dJ_dX[vi](base_row + 2, x0) += Dy(0);
			// X cordinate of the second vertex
			dJ_dX[vi](base_row + 0, x1) += Dx(1);
			dJ_dX[vi](base_row + 2, x1) += Dy(1);
			// X cordinate of the third vertex
			dJ_dX[vi](base_row + 0, x2) += Dx(2);
			dJ_dX[vi](base_row + 2, x2) += Dy(2);

			// Y cordinate of the first vertex
			dJ_dX[vi](base_row + 1, y0) += Dx(0);
			dJ_dX[vi](base_row + 3, y0) += Dy(0);
			// Y cordinate of the second vertex
			dJ_dX[vi](base_row + 1, y1) += Dx(1);
			dJ_dX[vi](base_row + 3, y1) += Dy(1);
			// Y cordinate of the third vertex
			dJ_dX[vi](base_row + 1, y2) += Dx(2);
			dJ_dX[vi](base_row + 3, y2) += Dy(2);
		}
	}
	
	prepare_hessian();
}

vector<int> OneRingAreaPreserving::get_one_ring_vertices(const vector<int>& OneRingFaces) {
	vector<int> vertices;
	vertices.clear();
	for (int i = 0; i < OneRingFaces.size(); i++) {
		int fi = OneRingFaces[i];
		int P0 = F(fi, 0);
		int P1 = F(fi, 1);
		int P2 = F(fi, 2);

		//check if the vertex already exist
		if (!(find(vertices.begin(), vertices.end(), P0) != vertices.end())) {
			vertices.push_back(P0);
		}
		if (!(find(vertices.begin(), vertices.end(), P1) != vertices.end())) {
			vertices.push_back(P1);
		}
		if (!(find(vertices.begin(), vertices.end(), P2) != vertices.end())) {
			vertices.push_back(P2);
		}
	}
	return vertices;
}

void OneRingAreaPreserving::updateX(const VectorXd& X)
{
	bool inversions_exist = updateJ(X);
	if (inversions_exist) {
		cout << name << " Error! inversion exists." << endl;
	}
}

void OneRingAreaPreserving::setVF(MatrixXd& V, MatrixX3i& F) {
	MatrixX3d V3d(V.rows(), 3);
	if (V.cols() == 2) {
		V3d.leftCols(2) = V;
		V3d.col(2).setZero();
	}
	else if (V.cols() == 3) {
		V3d = V;
	}
	this->V = V3d;
	this->F = F;
}

double OneRingAreaPreserving::value(bool update)
{
	double value = OneRingSum.cwiseAbs2().sum();
	value /= 2;

	if (update) {
		Efi.setZero();
		energy_value = value;
	}
	
	return value;
}

void OneRingAreaPreserving::gradient(VectorXd& g)
{
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	for (int vi = 0; vi < V.rows(); ++vi) {
		vector<int> OneRingFaces = VF[vi];
		vector<int> OneRingVertices = get_one_ring_vertices(OneRingFaces);

		int X_size = 2 * OneRingVertices.size();

		for (int xi = 0; xi < OneRingVertices.size(); xi++) {
			int global_xi = OneRingVertices[xi];
			g(global_xi) += grad[vi](xi);
			g(global_xi + V.rows()) += grad[vi](xi + OneRingVertices.size());
		}
	}
	gradient_norm = g.norm();
}

void OneRingAreaPreserving::hessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	for (int vi = 0; vi < V.rows(); ++vi) {
		vector<int> OneRingFaces = VF[vi];
		vector<int> OneRingVertices = get_one_ring_vertices(OneRingFaces);

		int X_size = 2 * OneRingVertices.size();

		MatrixXd Hi = Hessian[vi];
		for (int a = 0; a < X_size; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[index2++] = Hi(a, b);
			}
		}
		/*for (int xi = 0; xi < OneRingVertices.size(); xi++) {
			int global_xi = OneRingVertices[xi];
			g(global_xi) += grad[vi](xi);
			g(global_xi + V.rows()) += grad[vi](xi + OneRingVertices.size());
		}*/
		//cout << "Hi = " << endl << Hi << endl;
	}
	//cout << "II.size() = " << II.size() << endl;
	//cout << "JJ.size() = " << JJ.size() << endl;
	//cout << "SS.size() = " << SS.size() << endl;
	//for (int i = 0; i < II.size(); i++) {
	//	cout << "[" << II[i] << " , " << JJ[i] << "] = " << SS[i] << endl;
	//}
}


bool OneRingAreaPreserving::updateJ(const VectorXd& X)
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

	OneRingSum.setZero();
	for (int vi = 0; vi < VF.size(); vi++) {
		vector<int> OneRing = VF[vi];
		for (int fi : OneRing) {
			OneRingSum(vi) += Area(fi)*detJ(fi) - Area(fi);
		}
	}

	for (int vi = 0; vi < VF.size(); vi++) {
		vector<int> OneRingFaces = VF[vi];
		int J_size = 4 * OneRingFaces.size();

		MatrixXd dE_dJ(1, J_size);
		dE_dJ.setZero();

		MatrixXd d2E_dJ2(J_size, J_size);
		d2E_dJ2.setZero();
		
		//prepare gradient
		for (int i = 0; i < OneRingFaces.size(); i++) {
			int fi = OneRingFaces[i];
			int base_column = 4 * i;
			dE_dJ.block<1, 4>(0, base_column) = Area(fi)*Vector4d(d(fi), -c(fi), -b(fi), a(fi));
		}
		
		//prepare hessian
		for (int i = 0; i < OneRingFaces.size(); i++) {
			int fi = OneRingFaces[i];
			int base_row = 4 * i;

			
			d2E_dJ2.block(base_row + 0, 0, 1, J_size) = dE_dJ * Area(fi)*d(fi);
			d2E_dJ2(base_row + 0, base_row + 3) += OneRingSum(vi)*Area(fi);
			
			d2E_dJ2.block(base_row + 1, 0, 1, J_size) = -1 * dE_dJ * Area(fi)*c(fi);
			d2E_dJ2(base_row + 1, base_row + 2) -= OneRingSum(vi)*Area(fi);

			d2E_dJ2.block(base_row + 2, 0, 1, J_size) = -1 * dE_dJ * Area(fi)*b(fi);
			d2E_dJ2(base_row + 2, base_row + 1) -= OneRingSum(vi)*Area(fi);

			d2E_dJ2.block(base_row + 3, 0, 1, J_size) = dE_dJ * Area(fi)*a(fi);
			d2E_dJ2(base_row + 3, base_row + 0) += OneRingSum(vi)*Area(fi);
		}
		dE_dJ *= OneRingSum(vi);
		grad[vi] = dE_dJ * dJ_dX[vi];
		/*cout << "dE_dJ = " << endl << dE_dJ << endl;
		cout << "area = " << Area << endl;
		cout << "a = " << a << endl;
		cout << "b = " << b << endl;
		cout << "c = " << c << endl;
		cout << "d = " << d << endl;
		cout << "detj = " << detJ << endl;
		cout << "d2E_dJ2 = " << endl << d2E_dJ2 << endl;*/
		Hessian[vi] = dJ_dX[vi].transpose() * d2E_dJ2 * dJ_dX[vi];
	}
	

	return ((detJ.array() < 0).any());
}

void OneRingAreaPreserving::prepare_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) swap(i, j); II.push_back(i); JJ.push_back(j); };
	for (int vi = 0; vi < V.rows(); ++vi) {
		vector<int> OneRingFaces = VF[vi];
		vector<int> OneRingVertices = get_one_ring_vertices(OneRingFaces);
		int X_size = 2 * OneRingVertices.size();
		for (int a = 0; a < X_size; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				int global_a, global_b;

				if (a >= OneRingVertices.size()) {
					global_a = OneRingVertices[a - OneRingVertices.size()] + V.rows();
				}
				else {
					global_a = OneRingVertices[a];
				}
				if (b >= OneRingVertices.size()) {
					global_b = OneRingVertices[b - OneRingVertices.size()] + V.rows();
				}
				else {
					global_b = OneRingVertices[b];
				}
				//cout << "Ring " << vi << ": " << "(" << global_a << " , " << global_b << ")" << endl;
				PushPair(global_a, global_b);
			}
		}
	}
	SS = vector<double>(II.size(), 0.);
}