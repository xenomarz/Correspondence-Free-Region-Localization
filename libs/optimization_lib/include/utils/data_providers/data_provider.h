#pragma once
#ifndef OPTIMIZATION_LIB_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_DATA_PROVIDER_H

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "./mesh_data_provider.h"

class DataProvider
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
	virtual void Update(const Eigen::VectorXd& x) = 0;

protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
	
	// Elements count
	int64_t domain_faces_count_;
	int64_t domain_vertices_count_;
	int64_t image_faces_count_;
	int64_t image_vertices_count_;
	int64_t variables_count_;
};

#endif