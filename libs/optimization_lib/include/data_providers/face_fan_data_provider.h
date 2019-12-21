#pragma once
#ifndef OPTIMIZATION_LIB_FACE_FAN_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_FACE_FAN_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../core/core.h"
#include "./data_provider.h"

class FaceFanDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	FaceFanDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::FaceFan& face_fan);
	virtual ~FaceFanDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) override;

	/**
	 * Getters
	 */
	double GetAngle() const;
	const RDS::FaceFan& GetFaceFan() const;
	RDS::VertexIndex GetDomainVertexIndex() const;

private:
	RDS::FaceFan face_fan_;
	double angle_;
	RDS::VertexIndex domain_vertex_index_;
};

#endif