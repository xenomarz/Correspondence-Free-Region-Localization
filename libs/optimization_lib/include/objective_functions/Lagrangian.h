#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class Lagrangian : public TriangleMeshObjectiveFunction
{	
private:
	MatrixXd grad;
	VectorXd lambda;
	vector<Matrix<double, 6, 6>> Hessian;
	vector<Matrix<double, 4, 6>> dJ_dX;
	bool update_variables(const VectorXd& X);
	void init_hessian() override;
public:
	Lagrangian();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	double AugmentedValue();
};