#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class TotalObjective : public ObjectiveFunction
{
public:
	TotalObjective();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value() override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;

	// sub objectives
	vector<shared_ptr<ObjectiveFunction>> objectiveList;
};