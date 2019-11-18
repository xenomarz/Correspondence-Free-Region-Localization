#pragma once
#ifndef OPTIMIZATION_LIB_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_DATA_PROVIDER_H

// Optimization Lib Includes
#include "../core/updatable_object.h"
#include "./mesh_data_provider.h"

class DataProvider : public UpdatableObject
{
public:
	/**
	 * Constructors and destructor
	 */
	DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~DataProvider();

	/**
	 * Public methods
	 */
	[[nodiscard]] std::shared_ptr<MeshDataProvider> GetMeshDataProvider() const;

protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
};

#endif