#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class total_objective : public objective_function
{
private:
	virtual void init_hessian() override;
public:
	total_objective();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	
	// sub objectives
	float Shift_eigen_values = 1e-6;
	vector<shared_ptr<objective_function>> objectiveList;
};