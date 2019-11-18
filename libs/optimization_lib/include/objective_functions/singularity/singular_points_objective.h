#pragma once
#ifndef OPTIMIZATION_LIB_SINGULAR_POINTS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SINGULAR_POINTS_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./objective_functions/summation_objective.h"
#include "./objective_functions/singularity/singular_point_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SingularPointsObjective : public SummationObjective<SingularPointObjective<StorageOrder_>>
{
public:
	/**
	 * Constructors and destructor
	 */
	SingularPointsObjective(const std::string& name) :
		SummationObjective(name, true)
	{

	}

	SingularPointsObjective() :
		SummationObjective("Singular Points")
	{

	}

	virtual ~SingularPointsObjective()
	{

	}
};

#endif