#pragma once
#ifndef OPTIMIZATION_LIB_UPDATABLE_OBJECT_H
#define OPTIMIZATION_LIB_UPDATABLE_OBJECT_H

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/mesh_data_provider.h"

//#include <unordered_set>

class UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	enum class UpdateOptions : int32_t
	{
		NONE = 0,
		VALUE = 1,
		VALUE_PER_VERTEX = 2,
		GRADIENT = 4,
		HESSIAN = 8,
		ALL = 15
	};
	
	/**
	 * Constructors and destructor
	 */
	UpdatableObject(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~UpdatableObject();

	/**
	 * Public getters
	 */
	[[nodiscard]] std::shared_ptr<MeshDataProvider> GetMeshDataProvider() const;
	const std::vector<std::shared_ptr<UpdatableObject>>& GetDependencies() const;
	
	/**
	 * Public methods
	 */
	virtual void Initialize();
	virtual void Update(const Eigen::VectorXd& x) = 0;
	virtual void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) = 0;
	
protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
	std::vector<std::shared_ptr<UpdatableObject>> dependencies_;
	std::vector<std::vector<std::shared_ptr<UpdatableObject>>> dependency_layers_;

private:
	/**
	 * Private methods
	 */
	void InitializeDependencyLayers(std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers);
	int BuildDependencyLayers(const std::shared_ptr<UpdatableObject>& updatable_object, std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers) const;
};

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
UpdatableObject::UpdateOptions operator | (const UpdatableObject::UpdateOptions lhs, const UpdatableObject::UpdateOptions rhs);
UpdatableObject::UpdateOptions& operator |= (UpdatableObject::UpdateOptions& lhs, const UpdatableObject::UpdateOptions rhs);
UpdatableObject::UpdateOptions operator & (const UpdatableObject::UpdateOptions lhs, const UpdatableObject::UpdateOptions rhs);
UpdatableObject::UpdateOptions& operator &= (UpdatableObject::UpdateOptions& lhs, const UpdatableObject::UpdateOptions rhs);

#endif