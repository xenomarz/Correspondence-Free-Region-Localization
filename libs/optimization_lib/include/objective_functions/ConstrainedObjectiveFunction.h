#pragma once

#include "libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h"

class ConstrainedObjectiveFunction : public TriangleMeshObjectiveFunction
{
protected:
	Eigen::VectorXd lambda;
	//float augmented_value_parameter = 1;

	virtual bool update_variables(const Eigen::VectorXd& X) override;
	virtual void init_hessian() override;
	void init_aug_hessian();
	void AddElementToAugHessian(std::vector<int> ind);
public:
	// Hessian sparse reprensentation
	std::vector<int> II_aug, JJ_aug;
	std::vector<double> SS_aug;

	ConstrainedObjectiveFunction() {}
	virtual void init() override;

	//save values to show results
	double objective_value = 0;
	double constraint_value = 0;
	double objective_gradient_norm = 0;
	double constraint_gradient_norm = 0;
	float augmented_value_parameter = 100000.0f;
};
