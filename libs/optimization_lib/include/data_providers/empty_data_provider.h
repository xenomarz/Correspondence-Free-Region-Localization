#pragma once
#ifndef OPTIMIZATION_LIB_EMPTY_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_EMPTY_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"

class EmptyDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	EmptyDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~EmptyDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
};

#endif