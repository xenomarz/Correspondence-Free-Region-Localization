#pragma once
#ifndef OPTIMIZATION_LIB_UPDATABLE_OBJECT_H
#define OPTIMIZATION_LIB_UPDATABLE_OBJECT_H

// TBB includes
#include <tbb/concurrent_vector.h>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/mesh_data_provider.h"

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
	 * Public getters
	 */
	[[nodiscard]] std::shared_ptr<MeshDataProvider> GetMeshDataProvider() const;
	const tbb::concurrent_vector<std::shared_ptr<UpdatableObject>>& GetDependencies() const;
	
	/**
	 * Public methods
	 */
	virtual void Initialize();
	virtual void Update(const Eigen::VectorXd& x) = 0;
	virtual void Update(const Eigen::VectorXd& x, const int32_t update_modifiers) = 0;
	
protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
	tbb::concurrent_vector<std::shared_ptr<UpdatableObject>> dependencies_;
	std::vector<std::vector<std::shared_ptr<UpdatableObject>>> dependency_layers_;

private:
	/**
	 * Private methods
	 */
	void InitializeDependencyLayers(std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers);
	int BuildDependencyLayers(const std::shared_ptr<UpdatableObject>& updatable_object, std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers) const;
};

#endif