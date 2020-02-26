#pragma once
#ifndef OPTIMIZATION_LIB_FACE_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_FACE_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../data_providers/face_data_provider.h"
#include "../dense_objective_function.h"

template <Eigen::StorageOptions StorageOrder_>
class FacePositionObjective : public DenseObjectiveFunction<StorageOrder_>
{
public:
	
	/**
	 * Constructors and destructor
	 */
	FacePositionObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<FaceDataProvider>& face_data_provider, const std::string& name) :
		DenseObjectiveFunction(mesh_data_provider, face_data_provider, name, face_data_provider->GetFaceDescriptor().size(), false)
	{

	}

	virtual ~FacePositionObjective()
	{

	}

	/**
	 * Public methods
	 */
	virtual void MoveFacePosition(const Eigen::Vector2d& offset) = 0;

	[[nodiscard]] const std::shared_ptr<FaceDataProvider>& GetFaceDataProvider() const
	{
		return std::dynamic_pointer_cast<FaceDataProvider>(this->GetDataProvider());
	}

protected:
	
	/**
	 * Protected fields
	 */
	double coefficient_;
};

#endif