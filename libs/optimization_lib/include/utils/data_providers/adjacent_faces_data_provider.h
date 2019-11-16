#pragma once
#ifndef OPTIMIZATION_LIB_ADJACENT_FACES_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_ADJACENT_FACES_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../core/core.h"
#include "./data_provider.h"

class AdjacentFacesDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	AdjacentFacesDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::AdjacentFacesVertices& vertex_adjacent_faces_vertices);
	virtual ~AdjacentFacesDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;

	/**
	 * Getters
	 */
	double GetAngle() const;

private:
	RDS::AdjacentFacesVertices adjacent_faces_vertices_;
	double angle_;
};

#endif