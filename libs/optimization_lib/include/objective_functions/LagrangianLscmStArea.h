#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class LagrangianLscmStArea : public TriangleMeshObjectiveFunction
{	
private:
	VectorXd lambda;
	virtual bool update_variables(const VectorXd& X) override;
	virtual void init_hessian() override;
public:
	LagrangianLscmStArea();
	virtual void init() override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual double AugmentedValue() override;
};