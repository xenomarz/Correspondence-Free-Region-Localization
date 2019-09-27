#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class SymmetricDirichlet : public TriangleMeshObjectiveFunction
{	
private:
	VectorXd dirichlet;
	MatrixXd grad;
	vector<Matrix<double, 6, 6>> Hessian;
	bool update_variables(const VectorXd& X);
	void init_hessian() override;
public:
	SymmetricDirichlet();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};