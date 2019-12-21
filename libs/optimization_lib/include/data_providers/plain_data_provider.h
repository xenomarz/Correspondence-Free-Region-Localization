#pragma once
#ifndef OPTIMIZATION_LIB_PLAIN_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_PLAIN_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"

class PlainDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	PlainDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~PlainDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) override;
	const Eigen::VectorXd& GetX() const;

private:
	/**
	 * Private fields
	 */
	Eigen::VectorXd x_;
};

#endif