#pragma once
#include <libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h>

class LagrangianAreaStLscm : public ConstrainedObjectiveFunction
{	
public:
	LagrangianAreaStLscm();
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual double AugmentedValue() override;
};