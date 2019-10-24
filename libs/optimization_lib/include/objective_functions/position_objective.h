#pragma once
#ifndef OPTIMIZATION_LIB_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./concrete_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class PositionObjective : public ConcreteObjective<DenseObjectiveFunction<StorageOrder_>>
{
public:
	/**
	 * Constructors and destructor
	 */
	PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const int64_t vertices_count, const std::string& name) :
		ConcreteObjective(objective_function_data_provider, name),
		objective_vertices_count_(vertices_count),
		objective_variables_count_(2 * vertices_count)
	{

	}

	PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double coefficient, const int64_t vertices_count) :
		PositionObjective(objective_function_data_provider, coefficient, vertices_count, "Position Objective")
	{

	}

	virtual ~PositionObjective()
	{

	}

	/**
	 * Public methods
	 */
	virtual void OffsetPositionConstraint(const Eigen::Vector2d& offset) = 0;

protected:
	/**
	 * Protected fields
	 */
	int64_t objective_vertices_count_;
	int64_t objective_variables_count_;
	double coefficient_;
};

#endif