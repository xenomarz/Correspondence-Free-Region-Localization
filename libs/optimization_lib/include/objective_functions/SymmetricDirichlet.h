#pragma once
#include "libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h"

class SymmetricDirichlet : public TriangleMeshObjectiveFunction
{	
private:
	Eigen::VectorXd dirichlet;
	virtual bool update_variables(const Eigen::VectorXd& X) override;
public:
	SymmetricDirichlet();
	virtual void init() override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
};