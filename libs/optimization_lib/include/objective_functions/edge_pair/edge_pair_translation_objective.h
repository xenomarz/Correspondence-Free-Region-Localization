#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_TRANSLATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_TRANSLATION_OBJECTIVE_H

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../summation_objective.h"
#include "../cross_coordinate_diff_objective.h"
#include "../periodic_objective.h"
#include "../../data_providers/edge_pair_data_provider.h"

template <Eigen::StorageOptions StorageOrder_>
class EdgePairTranslationObjective : public SummationObjective<CrossCoordinateDiffObjective<StorageOrder_>, Eigen::SparseVector<double>>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairTranslationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider, const bool enforce_children_psd = true) :
		SummationObjective(mesh_data_provider, edge_pair_data_provider, "Edge Pair Translation Objective", enforce_children_psd)
	{
		this->Initialize();
	}

	virtual ~EdgePairTranslationObjective()
	{

	}

	/**
	 * Getters
	 */
	const EdgePairDataProvider& GetEdgePairDataProvider() const
	{
		return *std::dynamic_pointer_cast<EdgePairDataProvider>(this->data_provider_);
	}

	/**
	 * Public overrides
	 */
	void PreInitialize() override
	{
		auto edge_pair_data_provider = std::dynamic_pointer_cast<EdgePairDataProvider>(this->data_provider_);

		auto x_coordinate_diff_data_provider = std::make_shared<CrossCoordinateDiffDataProvider>(
			this->mesh_data_provider_,
			edge_pair_data_provider->GetEdge1Vertex1Index(),
			edge_pair_data_provider->GetEdge1Vertex2Index(),
			edge_pair_data_provider->GetEdge2Vertex1Index(),
			edge_pair_data_provider->GetEdge2Vertex2Index(),
			RDS::CoordinateType::X);
		
		auto y_coordinate_diff_data_provider = std::make_shared<CrossCoordinateDiffDataProvider>(
			this->mesh_data_provider_,
			edge_pair_data_provider->GetEdge1Vertex1Index(),
			edge_pair_data_provider->GetEdge1Vertex2Index(),
			edge_pair_data_provider->GetEdge2Vertex1Index(),
			edge_pair_data_provider->GetEdge2Vertex2Index(),
			RDS::CoordinateType::Y);
		
		auto x_coordinate_diff_objective = std::make_shared<CrossCoordinateDiffObjective<StorageOrder_>>(this->GetMeshDataProvider(), x_coordinate_diff_data_provider);
		auto y_coordinate_diff_objective = std::make_shared<CrossCoordinateDiffObjective<StorageOrder_>>(this->GetMeshDataProvider(), y_coordinate_diff_data_provider);

		this->AddObjectiveFunction(x_coordinate_diff_objective);
		this->AddObjectiveFunction(y_coordinate_diff_objective);
	}
};

#endif