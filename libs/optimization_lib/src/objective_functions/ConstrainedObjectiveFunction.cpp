#include "objective_functions/ConstrainedObjectiveFunction.h"

void ConstrainedObjectiveFunction::init()
{
	lambda.resize(F.rows());
	TriangleMeshObjectiveFunction::init();
	init_aug_hessian();
}

void ConstrainedObjectiveFunction::AddElementToAugHessian(std::vector<int> ind)
{
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II_aug.push_back(i); JJ_aug.push_back(j); };
	for (int i = 0; i < ind.size(); i++)
		for (int j = 0; j <= i; j++)
			PushPair(ind[i], ind[j]);
}

double ConstrainedObjectiveFunction::lagrangianValue(const bool update)
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

double ConstrainedObjectiveFunction::value(const bool update) {
	return lagrangianValue(update);
}

double ConstrainedObjectiveFunction::AugmentedValue(const bool update)
{
	double augmented = lagrangianValue(update) +
		(augmented_value_parameter / 2) * constrainedValue(update).cwiseAbs2().sum();

	return augmented;
}

void ConstrainedObjectiveFunction::gradient(Eigen::VectorXd& g, const bool update)
{
	lagrangianGradient(g, update);
}

void ConstrainedObjectiveFunction::init_aug_hessian() {
	II_aug.clear();
	JJ_aug.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II_aug.push_back(i); JJ_aug.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToAugHessian({ F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });

	// shift the diagonal of the hessian
	for (int i = 0; i < (2 * n); i++) {
		PushPair(i, i);
	}

	//we add the indexes of the last element in order to tell the solver the size of the matrix
	//PushPair(2 * n - 1, 2 * n - 1);

	SS_aug = std::vector<double>(II_aug.size(), 0.);
}

void ConstrainedObjectiveFunction::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i)
		AddElementToHessian(II, JJ, { F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });

	for (int i = 0; i < F.rows(); ++i)
	{
		PushPair(i + 2 * n, F(i, 0));
		PushPair(i + 2 * n, F(i, 1));
		PushPair(i + 2 * n, F(i, 2));
		PushPair(i + 2 * n, F(i, 0) + n);
		PushPair(i + 2 * n, F(i, 1) + n);
		PushPair(i + 2 * n, F(i, 2) + n);
	}
	//we add the indexes of the last element in order to tell the solver the size of the matrix
	PushPair(2 * n + F.rows() - 1, 2 * n + F.rows() - 1);

	SS = std::vector<double>(II.size(), 0.);
}

bool ConstrainedObjectiveFunction::update_variables(const Eigen::VectorXd& X)
{
	this->X = X;
	this->lambda = X.tail(F.rows());
	return TriangleMeshObjectiveFunction::update_variables(X.head(2 * V.rows()));
}

void ConstrainedObjectiveFunction::lagrangianGradient(Eigen::VectorXd& g, const bool update) {
	g.conservativeResize(V.rows() * 2 + F.rows());
	std::vector<int> I, J; std::vector<double> S;
	constrainedGradient(I, J, S);
	Eigen::SparseMatrix<double> ConstrGrad = Utils::BuildMatrix(I, J, S);

	g.head(2 * V.rows()) = objectiveGradient(update) - (ConstrGrad.transpose()*lambda);
	g.tail(F.rows()) = -1 * constrainedValue(update);

	//g.conservativeResize(V.rows() * 2 + F.rows());
	//g.setZero();

	//for (int fi = 0; fi < F.rows(); ++fi) {
	//	//prepare gradient
	//	Eigen::Vector4d dE_dJ(
	//		2 * a(fi) - 2 * d(fi) - lambda(fi) * d(fi),
	//		2 * b(fi) + 2 * c(fi) + lambda(fi) * c(fi),
	//		2 * b(fi) + 2 * c(fi) + lambda(fi) * b(fi),
	//		2 * d(fi) - 2 * a(fi) - lambda(fi) * a(fi)
	//	);
	//	grad.row(fi) = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();

	//	//Update the gradient of the x-axis
	//	g(F(fi, 0)) += grad(fi, 0);
	//	g(F(fi, 1)) += grad(fi, 1);
	//	g(F(fi, 2)) += grad(fi, 2);
	//	//Update the gradient of the y-axis
	//	g(F(fi, 0) + V.rows()) += grad(fi, 3);
	//	g(F(fi, 1) + V.rows()) += grad(fi, 4);
	//	g(F(fi, 2) + V.rows()) += grad(fi, 5);
	//	//Update the gradient of lambda
	//	g(fi + 2 * V.rows()) += Area(fi)*(1 - detJ(fi));
	//}

	if (update) {
		gradient_norm = g.norm();
		objective_gradient_norm = g.head(2 * V.rows()).norm();
		constraint_gradient_norm = g.tail(F.rows()).norm();
	}
}

void ConstrainedObjectiveFunction::AuglagrangGradWRTX(Eigen::VectorXd& g, const bool update) {
	std::vector<int> I, J; std::vector<double> S;
	constrainedGradient(I, J, S);
	Eigen::SparseMatrix<double> ConstrGrad = Utils::BuildMatrix(I, J, S);
	Eigen::VectorXd objGrad = objectiveGradient(update);
	Eigen::VectorXd constrGrad = constrainedValue(update);
	g = objGrad + (ConstrGrad.transpose()*(augmented_value_parameter*constrGrad - lambda));

	if (update) {
		gradient_norm = g.norm();
		//objective_gradient_norm = g.norm();
		//constraint_gradient_norm = g.tail(F.rows()).norm();
	}
}

void ConstrainedObjectiveFunction::hessian()
{
	/*
	*  Hess(L) = [Hess(f) - lambda*Hess(C)	,	-Grad(C).Transpose	]
	*			 [-Grad(C)					,	0					]
	*
	*  we build Hess(L) as an upper triangular matrix!
	*/
	std::vector<int> I, J;
	std::vector<double> S;
	std::vector<std::vector<int>> Is, Js;
	std::vector<std::vector<double>> Ss;

	/*
	* Adding the first part of the hessian => Hess(f) - lambda*Hess(C)
	*/
	// add Hess(f)
	objectiveHessian(II, JJ, SS);
	// add ( -lambda * Hess(C))
	constrainedHessian(Is, Js, Ss);
	for (int fi = 0; fi < F.rows(); fi++) {
		II.insert(II.end(), Is[fi].begin(), Is[fi].end());
		JJ.insert(JJ.end(), Js[fi].begin(), Js[fi].end());
		for (double val : Ss[fi])
			SS.push_back(val * (-lambda(fi)));
	}
	

	/////////////////////////////////////////////////////////////////
	// check point
	II.push_back(2 * V.rows() - 1);
	JJ.push_back(2 * V.rows() - 1);
	SS.push_back(0);

	Eigen::SparseMatrix<double> fullW, upperW =  Utils::BuildMatrix(II, JJ, SS);
	fullW = upperW.selfadjointView<Eigen::Upper>();
	W_k = fullW.toDense();

	std::cout << std::endl << std::endl << "---------------before----------------" << std::endl;
	std::cout << "W_k matrix is updated!" << std::endl;
	std::cout << "W_k: rows=" << W_k.rows() << " cols=" << W_k.cols() << std::endl;

	std::cout << "W_k: min eig value= " << W_k.eigenvalues().real().minCoeff() << std::endl;
	std::cout << "W_k: max eig value= " << W_k.eigenvalues().real().maxCoeff() << std::endl;
	//std::cout << "W_k: eig values= " << W_k.eigenvalues().real() << std::endl;
	/////////////////////////////////////////////////////////////////
	
	


	/*
	* Adding the second part of the hessian => -grad(C(x)).transpose()
	*/
	constrainedGradient(I, J, S);


	/////////////////////////////////////////////////////////////////
	// check point
	Eigen::FullPivLU<Eigen::MatrixXd> lu_decomp(A_k);
	auto rank = lu_decomp.rank();
	std::cout << "A_k: rank=" << rank << std::endl;

	Eigen::MatrixXd A_null_space = lu_decomp.kernel();
	Eigen::MatrixXd Roi = A_null_space.transpose() * W_k * A_null_space;
	double minEig = Roi.eigenvalues().real().minCoeff();
	std::cout << "before Roi: eig values= " << minEig << std::endl;
	std::cout << "A_k: Null space dim: rows=" << A_null_space.rows() << " , cols=" << A_null_space.cols() << std::endl;

	if (minEig < 0) {
		for (int i = 0; i < 2 * V.rows(); i++) {
			II.push_back(i);
			JJ.push_back(i);
			SS.push_back((1e-4) - (minEig));
		}
	}
	else {
		for (int i = 0; i < 2 * V.rows(); i++) {
			II.push_back(i);
			JJ.push_back(i);
			SS.push_back((1e-4));
		}
	}

	upperW = Utils::BuildMatrix(II, JJ, SS);
	fullW = upperW.selfadjointView<Eigen::Upper>();
	W_k = fullW.toDense();

	std::cout << std::endl << std::endl << "---------------after----------------" << std::endl;
	std::cout << "W_k matrix is updated!" << std::endl;
	std::cout << "W_k: rows=" << W_k.rows() << " cols=" << W_k.cols() << std::endl;

	std::cout << "W_k: min eig value= " << W_k.eigenvalues().real().minCoeff() << std::endl;
	std::cout << "W_k: max eig value= " << W_k.eigenvalues().real().maxCoeff() << std::endl;
	
	Roi = A_null_space.transpose() * W_k * A_null_space;
	minEig = Roi.eigenvalues().real().minCoeff();
	std::cout << "Roi: eig values= " << minEig << std::endl;


	/////////////////////////////////////////////////////////////////




	II.insert(II.end(), J.begin(), J.end());
	for (int i = 0; i < I.size(); i++) {
		JJ.push_back(I[i] + 2 * V.rows());
		SS.push_back(-S[i]);
	}

	/*
	* Tell the solver the size of the matrix
	*/
	II.push_back(2 * V.rows() + F.rows() - 1);
	JJ.push_back(2 * V.rows() + F.rows() - 1);
	SS.push_back(0);



	
	

	//std::cout << "(A_k * 1).sum=" << (A_k * Eigen::VectorXd::Ones(272)).sum() << std::endl;
	//std::cout << "1' * W_k * 1=" << Eigen::VectorXd::Ones(272).transpose() * W_k * Eigen::VectorXd::Ones(272) << std::endl;

	//for (int col = 0; col < A_null_space.cols(); col++) {
	//	Eigen::VectorXd d = A_null_space.col(col);
	//	std::cout << "Null space: column=" << col << std::endl;
	//	//std::cout << "Null space vector=" << d << std::endl;
	//	std::cout << "(A_k * d).sum=" << (A_k * d).sum() << std::endl;
	//	std::cout << "d' * W_k * d=" << d.transpose() * W_k * d << std::endl;

	//	assert((A_k * d).sum() < 1e-8 && (A_k * d).sum() > -(1e-8));
	//	assert(d.transpose() * W_k * d > 0);
	//}
	/////////////////////////////////////////////////////////////////


	//#pragma omp parallel for num_threads(24)
	//	int index2 = 0;
	//	for (int i = 0; i < F.rows(); ++i) {
	//		//prepare hessian
	//		Eigen::MatrixXd d2E_dJ2(4, 4);
	//		d2E_dJ2 <<
	//			2			, 0				, 0				, -lambda(i) -2,
	//			0			, 2				, 2+lambda(i)	, 0			,
	//			0			, 2 + lambda(i)	, 2				, 0			,
	//			-lambda(i)-2	, 0				, 0				, 2;
	//
	//		Hessian[i] = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
	//		
	//		for (int a = 0; a < 6; ++a)
	//		{
	//			for (int b = 0; b <= a; ++b)
	//			{
	//				SS[index2++] = Hessian[i](a, b);
	//			}
	//		}
	//	}
	//	
	//	for (int i = 0; i < F.rows(); ++i) {
	//		//prepare hessian
	//		Eigen::Vector4d dE_dJ(
	//			-d(i),
	//			c(i),
	//			b(i),
	//			-a(i)
	//		);
	//		Eigen::VectorXd hess = Area(i)*(dE_dJ.transpose() * dJ_dX[i]).transpose();
	//		SS[index2++] = hess[0];
	//		SS[index2++] = hess[1];
	//		SS[index2++] = hess[2];
	//		SS[index2++] = hess[3];
	//		SS[index2++] = hess[4];
	//		SS[index2++] = hess[5];	
	//	}
}

void ConstrainedObjectiveFunction::aughessian()
{
	std::vector<int> I, J;
	std::vector<double> S;
	std::vector<std::vector<int>> Is, Js;
	std::vector<std::vector<double>> Ss;

	// add Hess(f(x))
	objectiveHessian(II_aug, JJ_aug, SS_aug);

	// add (Meo * C - lambda) * Hess(C)
	constrainedHessian(Is, Js, Ss);
	Eigen::VectorXd constr = constrainedValue(true);
	for (int fi = 0; fi < F.rows(); fi++) {
		II_aug.insert(II_aug.end(), Is[fi].begin(), Is[fi].end());
		JJ_aug.insert(JJ_aug.end(), Js[fi].begin(), Js[fi].end());
		for (double val : Ss[fi])
			SS_aug.push_back(val * (augmented_value_parameter*constr(fi) - lambda(fi)));
	}

	// add Meo * grad(C) * grad(C)
	constrainedGradient(I, J, S);
	Eigen::SparseMatrix<double> res, ConstrGrad = Utils::BuildMatrix(I, J, S);
	res = augmented_value_parameter * ConstrGrad.transpose() * ConstrGrad;
	for (int k = 0; k < res.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(res, k); it; ++it)
		{
			if (it.col() >= it.row()) {
				II_aug.push_back(it.row());
				JJ_aug.push_back(it.col());
				SS_aug.push_back(it.value());
			}
		}
	}
	/*
	* Tell the solver the size of the matrix
	*/
	II_aug.push_back(2 * V.rows() - 1);
	JJ_aug.push_back(2 * V.rows() - 1);
	SS_aug.push_back(0);

	/*
	* Shift the diagonal of the hessian
	*/
	for (int i = 0; i < (2 * V.rows()); i++) {
		II_aug.push_back(i);
		JJ_aug.push_back(i);
		SS_aug.push_back(1e-6);
	}
	assert(SS_aug.size() == II_aug.size() && SS_aug.size() == JJ_aug.size());
}