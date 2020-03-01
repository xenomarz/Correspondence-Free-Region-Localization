#pragma once
#ifndef OPTIMIZATION_LIB_FACE_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_FACE_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../core/core.h"
#include "./data_provider.h"

class FaceDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	FaceDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::Face& face);
	virtual ~FaceDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, int32_t update_modifiers) override;

	/**
	 * Getters
	 */
	const RDS::Face& GetFace() const;
	const Eigen::VectorXd& GetBarycenter() const;

private:
	RDS::Face face_;
	Eigen::VectorXd barycenter_;
};

#endif