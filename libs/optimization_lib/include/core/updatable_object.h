#pragma once
#ifndef OPTIMIZATION_LIB_UPDATABLE_OBJECT_H
#define OPTIMIZATION_LIB_UPDATABLE_OBJECT_H

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "data_providers/mesh_data_provider.h"

//#include <unordered_set>

class UpdatableObject
{
public:
	/**
	 * Constructors and destructor
	 */
	UpdatableObject(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~UpdatableObject();

	/**
	 * Public methods
	 */
	virtual void Update(const Eigen::VectorXd& x) = 0;
	[[nodiscard]] std::shared_ptr<MeshDataProvider> GetMeshDataProvider() const;

protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
	std::vector<std::shared_ptr<UpdatableObject>> dependencies_;
};

#endif