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

Eigen::VectorXd LagrangianLscmStArea::objectiveGradient(const bool update) {
	Eigen::VectorXd obj;
	obj.conservativeResize(V.rows() * 2);
	obj.setZero();

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ(
			2 * a(fi) - 2 * d(fi),
			2 * b(fi) + 2 * c(fi),
			2 * b(fi) + 2 * c(fi),
			2 * d(fi) - 2 * a(fi)
		);
		grad.row(fi) = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();

		//Update the gradient of the x-axis
		obj(F(fi, 0)) += grad(fi, 0);
		obj(F(fi, 1)) += grad(fi, 1);
		obj(F(fi, 2)) += grad(fi, 2);
		//Update the gradient of the y-axis
		obj(F(fi, 0) + V.rows()) += grad(fi, 3);
		obj(F(fi, 1) + V.rows()) += grad(fi, 4);
		obj(F(fi, 2) + V.rows()) += grad(fi, 5);
	}
	if (update) {
		objective_gradient_norm = obj.norm();
	}
	return obj;
}

void LagrangianLscmStArea::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	int n = V.rows();
	for (int i = 0; i < F.rows(); ++i) {
		AddElementToHessian(I, J, { F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });
	}
	//we add the indexes of the last element in order to tell the solver the size of the matrix
	I.push_back(2 * n - 1);
	J.push_back(2 * n - 1);
	S = std::vector<double>(I.size(), 0.);
	
#pragma omp parallel for num_threads(24)
	int index = 0;
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		Eigen::MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			2	, 0	, 0	, - 2,
			0	, 2	, 2 , 0,
			0	, 2 , 2	, 0,
			- 2	, 0	, 0	, 2;

		Eigen::Matrix<double, 6, 6> curr_H = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];

		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				S[index++] = curr_H(a, b);
			}
		}
	}
}

void LagrangianLscmStArea::constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ(
			d(fi),
			-c(fi),
			-b(fi),
			a(fi)
		);
		Eigen::VectorXd curr_grad = Area(fi)*(dE_dJ.transpose() * dJ_dX[fi]).transpose();
		//Update the gradient of the x-axis
		PushTripple(fi, F(fi, 0), curr_grad(0));
		PushTripple(fi, F(fi, 1), curr_grad(1));
		PushTripple(fi, F(fi, 2), curr_grad(2));
		//Update the gradient of the y-axis
		PushTripple(fi, F(fi, 0) + V.rows(), curr_grad(3));
		PushTripple(fi, F(fi, 1) + V.rows(), curr_grad(4));
		PushTripple(fi, F(fi, 2) + V.rows(), curr_grad(5));
	}
	//we add the indexes of the last element in order to tell the solver the size of the matrix
	PushTripple(F.rows() - 1, 2 * V.rows() - 1, 0);
}

void LagrangianLscmStArea::constrainedHessian(std::vector<std::vector<int>>& Is, std::vector<std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) {
	Is.clear();	Js.clear();	Ss.clear();
	int n = V.rows();
	
#pragma omp parallel for num_threads(24)
	for (int i = 0; i < F.rows(); ++i) {
		std::vector<int> I, J;
		std::vector<double> S;
		AddElementToHessian(I, J, { F(i, 0), F(i, 1), F(i, 2), F(i, 0) + n, F(i, 1) + n, F(i, 2) + n });
		//we add the indexes of the last element in order to tell the solver the size of the matrix
		I.push_back(2 * n - 1);	J.push_back(2 * n - 1);

		//prepare hessian
		Eigen::MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			0, 0,  0,  1,
			0, 0,  -1, 0,
			0, -1, 0,  0,
			1, 0,  0,  0;

		Eigen::Matrix<double, 6, 6> curr_H = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];
		for (int a = 0; a < 6; ++a)
			for (int b = 0; b <= a; ++b)
				S.push_back(curr_H(a, b));
		S.push_back(0);

		Is.push_back(I);
		Js.push_back(J);
		Ss.push_back(S);
	}
}

Eigen::VectorXd LagrangianLscmStArea::constrainedValue(const bool update) {
	Eigen::VectorXd areaE = detJ - Eigen::VectorXd::Ones(F.rows());
	areaE = Area.asDiagonal() * areaE;
	if (update) {
		Efi = areaE.cwiseAbs2();
		constraint_value = areaE.cwiseAbs2().sum();
		constraint_gradient_norm = areaE.norm();
	}
	return areaE;
}

void LagrangianLscmStArea::lagrangianGradient(Eigen::VectorXd& g, const bool update) {
	g.conservativeResize(V.rows() * 2 + F.rows());
	std::vector<int> I, J; std::vector<double> S;
	constrainedGradient(I, J, S);
	Eigen::SparseMatrix<double> ConstrGrad = Utils::BuildMatrix(I, J, S);

	g.head(2*V.rows()) = objectiveGradient(update) - (ConstrGrad.transpose()*lambda);
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

void LagrangianLscmStArea::AuglagrangGradWRTX(Eigen::VectorXd& g, const bool update) {
	g.conservativeResize(V.rows() * 2);
	g.setZero();

	Eigen::VectorXd constr = constrainedValue(false);

	for (int fi = 0; fi < F.rows(); ++fi) {
		//prepare gradient
		Eigen::Vector4d dE_dJ(
			2 * a(fi) - 2 * d(fi) + ((augmented_value_parameter*constr(fi)) - lambda(fi)) * d(fi),
			2 * b(fi) + 2 * c(fi) - ((augmented_value_parameter*constr(fi)) - lambda(fi)) * c(fi),
			2 * b(fi) + 2 * c(fi) - ((augmented_value_parameter*constr(fi)) - lambda(fi)) * b(fi),
			2 * d(fi) - 2 * a(fi) + ((augmented_value_parameter*constr(fi)) - lambda(fi)) * a(fi)
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
	}
	if (update) {
		gradient_norm = g.norm();
		//objective_gradient_norm = g.norm();
		//constraint_gradient_norm = g.tail(F.rows()).norm();
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

void LagrangianLscmStArea::aughessian()
{
#pragma omp parallel for num_threads(24)
	int index2 = 0;
	
	for (int i = 0; i < F.rows(); ++i) {
		//prepare hessian
		float u = Area(i) * augmented_value_parameter;
		Eigen::MatrixXd d2E_dJ2(4, 4);
		d2E_dJ2 <<
			2 + u * d(i)*d(i)										, -u * d(i)*c(i)										, -u * d(i)*b(i)										, -lambda(i) - 2 - u + 2 * u * d(i)*a(i) - u * b(i)*c(i),
			-u * d(i)*c(i)											, 2 + u * c(i)*c(i)										, 2 + lambda(i) - u * a(i)*d(i) + 2 * u*b(i)*c(i) + u	, -u * c(i)*a(i)										,
			-u * d(i)*b(i)											, 2 + lambda(i) - u * a(i)*d(i) + 2 * u*b(i)*c(i) + u	, 2 + u * b(i)*b(i)										, -u * b(i)*a(i)										,
			-lambda(i) - 2 - u + 2 * u * d(i)*a(i) - u * b(i)*c(i)	, -u * c(i)*a(i)										, -u * b(i)*a(i)										, 2 + u * a(i)*a(i)										;

		Hessian[i] = Area(i) * dJ_dX[i].transpose() * d2E_dJ2 * dJ_dX[i];

		for (int a = 0; a < 6; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS_aug[index2++] = Hessian[i](a, b);
			}
		}
	}

	// shift the diagonal of the hessian
	int rows = *std::max_element(II_aug.begin(), II_aug.end()) + 1;
	for (int i = 0; i < rows; i++) {
		SS_aug[index2++] = 1e-6;
	}
	assert(SS_aug.size() == II_aug.size() && SS_aug.size() == JJ_aug.size());
}