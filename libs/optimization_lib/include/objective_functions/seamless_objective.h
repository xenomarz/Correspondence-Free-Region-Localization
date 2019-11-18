#pragma once
#ifndef OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./summation_objective.h"
#include "./edge_pair/edge_pair_angle_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SeamlessObjective : public SummationObjective<EdgePairAngleObjective<StorageOrder_>>
{
public:
	/**
	 * Constructors and destructor
	 */
	SeamlessObjective(const std::string& name) :
		SummationObjective(name, true)
	{

	}

	SeamlessObjective() :
		SeamlessObjective("Seamless")
	{

	}

	virtual ~SeamlessObjective()
	{

	}
};

#endif