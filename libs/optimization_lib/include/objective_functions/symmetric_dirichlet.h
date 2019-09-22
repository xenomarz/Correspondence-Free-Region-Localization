#pragma once

#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class SymmetricDirichlet : public TriangleMeshObjectiveFunction
{	
private:
	// Jacobian determinant (ad-bc)
	VectorXd dirichlet; //Forbinous norm of the jacobian
	MatrixXd grad;
	vector<Matrix<double, 6, 6>> Hessian;
	vector<Matrix<double, 4, 6>> dJ_dX;

	virtual void init_hessian() override;
	bool update_variables(const VectorXd& X);

public:
	SymmetricDirichlet();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};