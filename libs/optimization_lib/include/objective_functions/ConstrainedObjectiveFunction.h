#pragma once

#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class ConstrainedObjectiveFunction : public TriangleMeshObjectiveFunction
{
protected:
	VectorXd lambda;
	//float augmented_value_parameter = 1;

	virtual bool update_variables(const VectorXd& X) override;
	virtual void init_hessian() override;
public:
	ConstrainedObjectiveFunction() {}
	virtual void init() override;
};
