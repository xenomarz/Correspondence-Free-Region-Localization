#pragma once

#include <libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h>

class TotalObjective : public ConstrainedObjectiveFunction
{
private:
	virtual void init_hessian() override;
public:
	TotalObjective();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual double AugmentedValue() override;

	// sub objectives
	float Shift_eigen_values = 0;
	vector<shared_ptr<ObjectiveFunction>> objectiveList;
};