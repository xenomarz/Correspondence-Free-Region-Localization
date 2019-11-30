#pragma once
#ifndef OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/empty_data_provider.h"
#include "../data_providers/edge_pair_data_provider.h"
#include "./summation_objective.h"
#include "./edge_pair/edge_pair_objective.h"
#include "./edge_pair/edge_pair_angle_objective.h"
#include "./edge_pair/edge_pair_length_objective.h"
#include "../objective_functions/periodic_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SeamlessObjective : public SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>
{
public:
	/**
	 * Constructors and destructor
	 */
	SeamlessObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::string& name, const bool enforce_children_psd = true) :
		SummationObjective(empty_data_provider, name, false, enforce_children_psd, false)
	{

	}

	SeamlessObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const bool enforce_children_psd = true) :
		SeamlessObjective(empty_data_provider, "Seamless", enforce_children_psd)
	{

	}

	virtual ~SeamlessObjective()
	{

	}

	/**
	 * Public methods
	 */
	void AddEdgePairObjectives(const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider)
	{	
		std::shared_ptr<EdgePairAngleObjective<StorageOrder_>> edge_pair_angle_objective = std::make_shared<EdgePairAngleObjective<StorageOrder_>>(edge_pair_data_provider, false);
		std::shared_ptr<EdgePairLengthObjective<StorageOrder_>> edge_pair_length_objective = std::make_shared<EdgePairLengthObjective<StorageOrder_>>(edge_pair_data_provider, this->GetEnforceChildrenPsd());

		double period = M_PI / 2;
		std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_edge_pair_angle_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(edge_pair_angle_objective, period, this->GetEnforceChildrenPsd());

		this->AddObjectiveFunction(periodic_edge_pair_angle_objective);
		this->AddObjectiveFunction(edge_pair_length_objective);
	}
};

#endif