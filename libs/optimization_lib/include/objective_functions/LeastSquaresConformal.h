#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class LeastSquaresConformal : public TriangleMeshObjectiveFunction
{	
public:
	LeastSquaresConformal();
	virtual void init() override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};