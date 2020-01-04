#pragma once
#include <libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h>

class LagrangianAreaStLscm : public ConstrainedObjectiveFunction
{	
public:
	LagrangianAreaStLscm();
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
	virtual double AugmentedValue(const bool update) override;
};