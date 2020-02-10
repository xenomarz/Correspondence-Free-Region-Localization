#include "objective_functions/TestConstrainedRosenbrock.h"

TestConstrainedRosenbrock::TestConstrainedRosenbrock()
{
	name = "Test - constrained Rosenbrock";
	w = 1.0f;
}

double TestConstrainedRosenbrock::objectiveValue(const bool update) {
	double objVal = pow(1 - X(0), 2) + 100 * pow(X(1)-X(0)*X(0), 2);
	if (update) {
		objective_value = objVal;
	}
	return objVal;
}

Eigen::VectorXd TestConstrainedRosenbrock::constrainedValue(const bool update) {
	Eigen::VectorXd constr(F.rows());
	constr(0) = X(1) - X(0) - 5;
	if (update) {
		Efi = constr.cwiseAbs2();
		constraint_value = constr.cwiseAbs2().sum();
	}
	return constr;
}

Eigen::VectorXd TestConstrainedRosenbrock::objectiveGradient(const bool update) {
	Eigen::VectorXd grad;
	grad <<
		2 * (X(0) - 1) - 400 * (X(1) - X(0)*X(0))*X(0),
		200 * (X(1) - X(0)*X(0));
	return grad;
}

void TestConstrainedRosenbrock::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };
	PushTripple(0, 0, 2 - 400 * X(1) + 12 * X(0)*X(0) - 400 * X(0));
	PushTripple(0, 1, -400 * X(0));
	PushTripple(1, 0, -400 * X(0));
	PushTripple(1, 1, 200);
	
	//Tell the solver the size of the matrix
	PushTripple(2 * V.rows() - 1, 2 * V.rows() - 1, 0);
}

void TestConstrainedRosenbrock::constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };

	PushTripple(0, 0, -1);
	PushTripple(0, 1, 1);
	
	//Tell the solver the size of the matrix
	PushTripple(F.rows() - 1, 2 * V.rows() - 1, 0);
}

void TestConstrainedRosenbrock::constrainedHessian(std::vector<std::vector<int>>& Is, std::vector < std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) {
	Is.clear();	Js.clear();	Ss.clear();
	std::vector<int> I, J;
	std::vector<double> S;
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };
	
	//////////////////////////////////////////
	//Add the first constrained matrix
	I.clear();	J.clear();	S.clear();
	
	//Tell the solver the size of the matrix
	PushTripple(2 * V.rows() - 1,2 * V.rows() - 1,0);

	Is.push_back(I);
	Js.push_back(J);
	Ss.push_back(S);
}
