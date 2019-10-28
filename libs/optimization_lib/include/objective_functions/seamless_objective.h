#pragma once
#ifndef OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./composite_objective.h"
#include "./edge_pair_angle_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SeamlessObjective : public CompositeObjective<EdgePairAngleObjective<StorageOrder_>>
{
public:
	/**
	 * Constructors and destructor
	 */
	SeamlessObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		CompositeObjective(objective_function_data_provider, name, false, true)
	{

	}

	SeamlessObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
		SeamlessObjective(objective_function_data_provider, "Seamless")
	{

	}

	virtual ~SeamlessObjective()
	{

	}

	/**
	 * Public methods
	 */
	void AddCorrespondingEdgePair(const std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>& corresponding_edge_pair)
	{
		corresponding_edge_pairs_.push_back(corresponding_edge_pair);
		this->AddObjectiveFunction(std::make_shared<EdgePairAngleObjective<StorageOrder_>>(this->objective_function_data_provider_, corresponding_edge_pair.first, corresponding_edge_pair.second));
	}

	void AddCorrespondingEdgePairs(const std::vector<std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>>& corresponding_edge_pairs)
	{
		for (auto& corresponding_edge_pair : corresponding_edge_pairs)
		{
			corresponding_edge_pairs_.push_back(corresponding_edge_pair);
			this->AddObjectiveFunction(std::make_shared<EdgePairAngleObjective<StorageOrder_>>(this->objective_function_data_provider_, corresponding_edge_pair.first, corresponding_edge_pair.second));
		}
	}

private:
	std::vector<std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>> corresponding_edge_pairs_;
};

#endif