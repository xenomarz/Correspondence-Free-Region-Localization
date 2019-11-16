#pragma once
#ifndef OPTIMIZATION_LIB_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../data_providers/plain_data_provider.h"
#include "../dense_objective_function.h"

template <Eigen::StorageOptions StorageOrder_>
class PositionObjective : public DenseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	PositionObjective(const std::shared_ptr<PlainDataProvider>& plain_data_provider, const std::string& name, const int64_t objective_vertices_count) :
		DenseObjectiveFunction(plain_data_provider, name, objective_vertices_count, false)
	{

	}

	PositionObjective(const int64_t objective_vertices_count) :
		PositionObjective("Position Objective", objective_vertices_count)
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
	double coefficient_;
};

#endif