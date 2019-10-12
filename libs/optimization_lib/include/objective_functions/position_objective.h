#pragma once
#ifndef OPTIMIZATION_LIB_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./objective_function.h"
#include <Eigen/src/Core/util/ForwardDeclarations.h>
#include <Eigen/src/Core/util/ForwardDeclarations.h>

class PositionObjective : public ObjectiveFunction
{
public:
	/**
	 * Constructors and destructor
	 */
	PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double coefficient, const int64_t vertices_count, const std::string& name);
	PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double coefficient, const int64_t vertices_count);
	virtual ~PositionObjective();

	/**
	 * Public methods
	 */
	virtual void OffsetPositionConstraint(const Eigen::Vector2d& offset) = 0;

protected:
	/**
	 * Protected fields
	 */
	int64_t vertices_count_;
	double coefficient_;
	
private:
	/**
	 * Overrides
	 */
	virtual void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) = 0;
	virtual void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) = 0;
	virtual void CalculateGradient(Eigen::VectorXd& g) = 0;
	virtual void CalculateHessian(std::vector<double>& ss) = 0;
	virtual void PreUpdate(const Eigen::VectorXd& x) = 0;
};

#endif