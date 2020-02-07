#pragma once
#include "libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h"

class PenaltyPositionalConstraints : public ConstrainedObjectiveFunction
{
private:
	virtual void init_hessian() override;
public:
	PenaltyPositionalConstraints(bool isConstrObjFunc);
	virtual void init() override;
	virtual void updateX(const Eigen::VectorXd& X) override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
	virtual double AugmentedValue(const bool update) override;

	virtual double objectiveValue(const bool update) override;
	virtual Eigen::VectorXd objectiveGradient(const bool update) override;
	virtual void objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual Eigen::VectorXd constrainedValue(const bool update) override;
	virtual void constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual void constrainedHessian(std::vector<std::vector<int>>& Is, std::vector < std::vector<int>>& Js, std::vector < std::vector<double>>& Ss) override;
	virtual void lagrangianGradient(Eigen::VectorXd& g, const bool update) override;

	std::vector<int> ConstrainedVerticesInd;
	Eigen::MatrixX2d ConstrainedVerticesPos;
	Eigen::MatrixX2d CurrConstrainedVerticesPos;
	int numV=0;
	int numF=0;
	bool IsConstrObjFunc;
};