#include "objective_functions/TestConstrainedFunction.h"

TestConstrainedFunction::TestConstrainedFunction()
{
	name = "Test - constrained Function";
	w = 1.0f;
}

double TestConstrainedFunction::objectiveValue(const bool update) {
	double objVal = X(0)*X(0) + 2 * X(1)*X(1) - X(2) + X(3)*X(3);
	if (update) {
		objective_value = objVal;
	}
	return objVal;
}

Eigen::VectorXd TestConstrainedFunction::constrainedValue(const bool update) {
	Eigen::VectorXd constr(F.rows());
	constr(0) = X(0)*X(0) + X(2)*X(2) + X(0)*X(2) - 1;
	constr(1) = X(2) + X(3);
	if (update) {
		Efi = constr.cwiseAbs2();
		constraint_value = constr.cwiseAbs2().sum();
	}
	return constr;
}

Eigen::VectorXd TestConstrainedFunction::objectiveGradient(const bool update) {
	Eigen::VectorXd grad;
	grad << 2 * X(0), 4 * X(1), -1, 2 * X(3);
	return grad;
}

void TestConstrainedFunction::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };
	PushTripple(0,0,2);
	PushTripple(1,1,4);
	PushTripple(3,3,2);
	
	//Tell the solver the size of the matrix
	PushTripple(2 * V.rows() - 1, 2 * V.rows() - 1, 0);
}

void TestConstrainedFunction::constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	I.clear();	J.clear();	S.clear();
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };

	PushTripple(0,0, 2*X(0)+X(2));
	PushTripple(0,2, 2*X(2)+X(0));
	PushTripple(1,2, 1);
	PushTripple(1,3, 1);

	//Tell the solver the size of the matrix
	PushTripple(F.rows() - 1, 2 * V.rows() - 1, 0);
}

void TestConstrainedFunction::constrainedHessian(std::vector<std::vector<int>>& Is, std::vector < std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) {
	Is.clear();	Js.clear();	Ss.clear();
	std::vector<int> I, J;
	std::vector<double> S;
	auto PushTripple = [&](int i, int j, double v) { I.push_back(i); J.push_back(j); S.push_back(v); };
	
	//////////////////////////////////////////
	//Add the first constrained matrix
	I.clear();	J.clear();	S.clear();
	PushTripple(0, 0, 2);
	PushTripple(0, 2, 1);
	PushTripple(2, 0, 1);
	PushTripple(2, 2, 2);
	//Tell the solver the size of the matrix
	PushTripple(2 * V.rows() - 1,2 * V.rows() - 1,0);

	Is.push_back(I);
	Js.push_back(J);
	Ss.push_back(S);

	//////////////////////////////////////////
	//Add the second constrained matrix
	I.clear();	J.clear();	S.clear();
	
	//Tell the solver the size of the matrix
	PushTripple(2 * V.rows() - 1, 2 * V.rows() - 1, 0);

	Is.push_back(I);
	Js.push_back(J);
	Ss.push_back(S);
}
