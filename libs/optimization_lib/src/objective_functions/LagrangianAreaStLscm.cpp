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

void LagrangianAreaStLscm::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	II.clear();
	JJ.clear();
	auto PushPair = [&](int i, int j) { if (i > j) std::swap(i, j); II.push_back(i); JJ.push_back(j); };
	int n = V.rows();

	PushPair(0, 0);
	PushPair(0, 2);
	PushPair(0, 4);
	PushPair(1, 1);
	PushPair(2, 2);
	PushPair(2, 4);
	PushPair(2, 5);
	PushPair(3, 3);
	PushPair(3, 5);

	//we add the indexes of the last element in order to tell the solver the size of the matrix
	PushPair(2 * n + F.rows() - 1, 2 * n + F.rows() - 1);

	SS = std::vector<double>(II.size(), 0.);
}

void LagrangianAreaStLscm::constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	
}

void LagrangianAreaStLscm::constrainedHessian(std::vector<std::vector<int>>& Is, std::vector < std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) {
	
}
