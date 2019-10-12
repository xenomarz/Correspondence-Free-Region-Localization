#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./objective_function.h"
#include <Eigen/src/Core/util/ForwardDeclarations.h>
#include <Eigen/src/Core/util/ForwardDeclarations.h>

class CompositeObjective : public ObjectiveFunction
{
public:
	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function, const std::string& name);
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions, const std::string& name);
	virtual ~CompositeObjective();

	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction>& objective_function);
	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions);
	void RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunction>& objective_function);
	void RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions);
	const std::uint32_t GetObjectiveFunctionsCount() const;
	const std::shared_ptr<ObjectiveFunction> GetObjectiveFunction(std::uint32_t index) const;

private:

	/**
	 * Private overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) override;
	void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) override;
	void CalculateGradient(Eigen::VectorXd& g) override;
	void CalculateHessian(std::vector<double>& ss) override;
	void PreUpdate(const Eigen::VectorXd& x) override;
	void PreInitialize() override;

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunction>> objective_functions_;
};

#endif