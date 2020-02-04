#pragma once
#include "libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h"

class LagrangianAreaStLscm : public ConstrainedObjectiveFunction
{	
public:
	virtual void init_hessian() override;
	LagrangianAreaStLscm();
	virtual void hessian() override;
	
	virtual double objectiveValue(const bool update) override;
	virtual Eigen::VectorXd objectiveGradient(const bool update) override;
	virtual Eigen::SparseMatrix<double> objectiveHessian(const bool update) override;
	virtual Eigen::VectorXd constrainedValue(const bool update) override;
	virtual Eigen::SparseMatrix<double> constrainedGradient(const bool update) override;
	virtual std::vector<Eigen::SparseMatrix<double>> constrainedHessian(const bool update) override;
	virtual void lagrangianGradient(Eigen::VectorXd& g, const bool update) override;

	void AuglagrangGradWRTX(Eigen::VectorXd& g, const bool update);
	void aughessian();
};