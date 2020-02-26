#pragma once
#ifndef OPTIMIZATION_LIB_PATCH_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_PATCH_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../data_providers/face_data_provider.h"
#include "../../data_providers/empty_data_provider.h"
#include "../summation_objective.h"
#include "face_position_objective.h"
#include "../dense_objective_function.h"

template <Eigen::StorageOptions StorageOrder_>
class PatchPositionObjective : public SummationObjective<FacePositionObjective<StorageOrder_>, Eigen::VectorXd>
{
public:
	
	/**
	 * Constructors and destructor
	 */
	PatchPositionObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::string& name) :
		SummationObjective(mesh_data_provider, std::make_shared<EmptyDataProvider>(mesh_data_provider), name, false)
	{

	}

	virtual ~PatchPositionObjective()
	{

	}

	/**
	 * Public methods
	 */
	void MovePatchPosition(const Eigen::Vector2d& offset)
	{
		for(std::size_t i = 0; i < this->GetObjectiveFunctionsCount(); i++)
		{
			auto face_position_objective = this->GetObjectiveFunction(i);
			face_position_objective->MoveFacePosition(offset);
		}
	}
};

#endif