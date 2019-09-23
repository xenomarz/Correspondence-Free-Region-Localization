#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./objective_function.h"

class CompositeObjective : public ObjectiveFunction
{
public:

	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions);
	virtual ~CompositeObjective();

	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction> objective_function);
	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions);

private:

	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::VectorXd& X, double& f);
	void CalculateGradient(const Eigen::VectorXd& X, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::VectorXd& X, std::vector<double>& ss);
	void Update(const Eigen::VectorXd& X);

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunction>> objective_functions_;
};

#endif