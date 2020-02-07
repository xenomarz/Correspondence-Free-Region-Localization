#include "objective_functions/LagrangianAreaStLscm.h"

LagrangianAreaStLscm::LagrangianAreaStLscm()
{
	name = "TestFunction";
	w = 1.0f;
}

double LagrangianAreaStLscm::objectiveValue(const bool update) {
	
	double objVal = X(0)*X(0) + 2 * X(1)*X(1) - X(2) + X(3)*X(3);
	if (update) {
		objective_value = objVal;
	}
	return objVal;
}

Eigen::VectorXd LagrangianAreaStLscm::constrainedValue(const bool update) {
	Eigen::VectorXd constr(F.rows());
	constr(0) = X(0)*X(0) + X(2)*X(2) + X(0)*X(2) - 1;
	constr(1) = X(2) + X(3);
	if (update) {
		Efi = constr.cwiseAbs2();
		constraint_value = constr.cwiseAbs2().sum();
	}
	return constr;
}

Eigen::VectorXd LagrangianAreaStLscm::objectiveGradient(const bool update) {
	Eigen::VectorXd w;
	return w;
}

Eigen::SparseMatrix<double> LagrangianAreaStLscm::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	Eigen::SparseMatrix<double> w;
	return w;
}

Eigen::SparseMatrix<double> LagrangianAreaStLscm::constrainedGradient(const bool update) {
	Eigen::SparseMatrix<double> w;
	return w;
}

std::vector<Eigen::SparseMatrix<double>> LagrangianAreaStLscm::constrainedHessian(const bool update) {
	std::vector<Eigen::SparseMatrix<double>> w;
	return w;
}

void LagrangianAreaStLscm::lagrangianGradient(Eigen::VectorXd& g, const bool update) {
	g.conservativeResize(V.rows() * 2 + F.rows());
	g.setZero();

	g(0) = 2 * X(0) - lambda(0)*(2 * X(0) + X(2));
	g(1) = 4 * X(1);
	g(2) = -1 - lambda(0)*(2 * X(2) + X(0)) - lambda(1);
	g(3) = 2 * X(3) - lambda(1);
	g(4) = -1 * (X(0)*X(0) + X(2)*X(2) + X(0)*X(2) - 1);
	g(5) = -1 * (X(2) + X(3));

	if (update) {
		gradient_norm = g.norm();
		objective_gradient_norm = g.head(2 * V.rows()).norm();
		constraint_gradient_norm = g.tail(F.rows()).norm();
	}
}

void LagrangianAreaStLscm::AuglagrangGradWRTX(Eigen::VectorXd& g, const bool update) {
	std::cout << "Error 0" << std::endl;
}

void LagrangianAreaStLscm::init_hessian()
{
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();
	
	PushPair(0,0);
	PushPair(0,2);
	PushPair(0,4);
	PushPair(1,1);
	PushPair(2,2);
	PushPair(2,4);
	PushPair(2,5);
	PushPair(3,3);
	PushPair(3,5);

	//we add the indexes of the last element in order to tell the solver the size of the matrix
	PushPair(2 * n + F.rows() - 1, 2 * n + F.rows() - 1);

	SS = std::vector<double>(II.size(), 0.);
}

void LagrangianAreaStLscm::hessian()
{
	SS[0] = 2 - 2 * lambda(0);
	SS[1] = -lambda(0);
	SS[2] = -2 * X(0) - X(2);
	SS[3] = 4;
	SS[4] = -2 * lambda(0);
	SS[5] = -2*X(2)-X(0);
	SS[6] = -1;
	SS[7] = 2;
	SS[8] = -1;
}

void LagrangianAreaStLscm::aughessian()
{
	std::cout << "Error 1" << std::endl;
}