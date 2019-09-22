#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class AreaPreserving : public ObjectiveFunction
{	
private:
	// Jacobian determinant (ad-bc)
	MatrixXd grad;
	vector<Matrix<double, 6, 6>> Hessian;
	vector<Matrix<double, 4, 6>> dJ_dX;

public:
	AreaPreserving();

	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;
	bool updateJ(const VectorXd& X);
};