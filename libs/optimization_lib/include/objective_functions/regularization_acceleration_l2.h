#pragma once

#include <objective_functions/objective_function.h>

class RegularizationAccelerationL2 : public ObjectiveFunction
{
public:
	RegularizationAccelerationL2();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value() override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;
	
    MatrixXd AX,AY;  // holds the accelerations
    int timeSteps=0;
    int numV=0;
};