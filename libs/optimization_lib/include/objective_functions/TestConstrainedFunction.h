#pragma once
#include "libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h"

class TestConstrainedFunction : public ConstrainedObjectiveFunction
{	
public:
	TestConstrainedFunction();
	
	virtual double objectiveValue(const bool update) override;
	virtual Eigen::VectorXd objectiveGradient(const bool update) override;
	virtual void objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual Eigen::VectorXd constrainedValue(const bool update) override;
	virtual void constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual void constrainedHessian(std::vector<std::vector<int>>& Is, std::vector < std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) override;
};