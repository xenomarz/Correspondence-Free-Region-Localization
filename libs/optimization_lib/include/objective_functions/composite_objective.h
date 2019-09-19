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
	 * Destructor
	 */
	virtual ~CompositeObjective();

	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction> objective_function);

private:

	/**
	 * Friend classes
	 */
	friend class ObjectiveFunction;

	/**
	 * Constructors
	 */
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);


	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& X, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss);
	void Update(const Eigen::MatrixX2d& X);

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunction>> objective_functions_;
};

#endif