#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class SymmetricDirichlet : public TriangleMeshObjectiveFunction
{	
private:
	VectorXd dirichlet;
	virtual bool update_variables(const VectorXd& X) override;
public:
	SymmetricDirichlet();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};