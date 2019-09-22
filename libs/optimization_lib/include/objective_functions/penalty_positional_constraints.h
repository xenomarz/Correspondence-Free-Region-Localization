#pragma once
#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class PenaltyPositionalConstraints : public ObjectiveFunction
{
private:
	virtual void init_hessian() override;
public:
	PenaltyPositionalConstraints();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(Eigen::VectorXd& g) override;
	virtual void hessian() override;
	
	vector<int> ConstrainedVerticesInd;
	MatrixX2d ConstrainedVerticesPos;
	MatrixX2d CurrConstrainedVerticesPos;
	int numV=0;
};