#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class PenaltyPositionalConstraints : public ObjectiveFunction
{
public:
	PenaltyPositionalConstraints();

	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value() override;
	virtual void gradient(Eigen::VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;

	std::vector<int> ConstrainedVerticesInd;
	Eigen::MatrixX2d ConstrainedVerticesPos;
	Eigen::MatrixX2d CurrConstrainedVerticesPos;
	int numV=0;
};