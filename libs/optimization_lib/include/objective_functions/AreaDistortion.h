#pragma once
#include "libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h"

class AreaDistortion : public TriangleMeshObjectiveFunction
{	
public:
	AreaDistortion();
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g , const bool update) override;
	virtual void hessian() override;
};