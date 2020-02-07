#pragma once

#include "libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h"
#include "libs/optimization_lib/include/objective_functions/LagrangianAreaStLscm.h"
#include "libs/optimization_lib/include/objective_functions/LagrangianLscmStArea.h"

class TotalObjective : public ConstrainedObjectiveFunction
{
private:
	virtual void init_hessian() override;
public:
	TotalObjective();
	virtual void init() override;
	virtual void updateX(const Eigen::VectorXd& X) override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
	virtual double AugmentedValue(const bool update) override;

	virtual double objectiveValue(const bool update) override;
	virtual Eigen::VectorXd objectiveGradient(const bool update) override;
	virtual void objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual Eigen::VectorXd constrainedValue(const bool update) override;
	virtual void constrainedGradient(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) override;
	virtual std::vector<Eigen::SparseMatrix<double>> constrainedHessian(const bool update) override;
	virtual void lagrangianGradient(Eigen::VectorXd& g, const bool update) override;
	
	// sub objectives
	float Shift_eigen_values = 0;
	std::vector<std::shared_ptr<ObjectiveFunction>> objectiveList;
};