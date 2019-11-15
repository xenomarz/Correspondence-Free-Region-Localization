#pragma once
#ifndef OPTIMIZATION_LIB_FACE_FAN_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_FACE_FAN_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"
#include "../../utils/type_definitions.h"

class FaceFanDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	FaceFanDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex_index, const RDS::VertexAdjacentFacesVertices& vertex_adjacent_faces_vertices);
	virtual ~FaceFanDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;

	/**
	 * Getters
	 */
	double GetAngle() const;

private:
	RDS::VertexIndex vertex_index_;
	RDS::VertexAdjacentFacesVertices vertex_adjacent_faces_vertices_;
	double angle_;
};

#endif