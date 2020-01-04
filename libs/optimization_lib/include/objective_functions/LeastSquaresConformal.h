#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class LeastSquaresConformal : public TriangleMeshObjectiveFunction
{	
public:
	LeastSquaresConformal();
	virtual void init() override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
};