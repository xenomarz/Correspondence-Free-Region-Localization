#pragma once
#ifndef OPTIMIZATION_LIB_COORDINATE_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_COORDINATE_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"

class CoordinateDataProvider : public DataProvider
{
public:
	/**
	 * Public type definitions
	 */
	enum class CoordinateType : int32_t
	{
		X,
		Y
	};
	
	/**
	 * Constructors and destructor
	 */
	CoordinateDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex_index, const CoordinateType coordinate_type);
	virtual ~CoordinateDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) override;
	double GetCoordinateValue() const;
	RDS::SparseVariableIndex GetSparseVariableIndex() const;
	
private:
	/**
	 * Private fields
	 */
	RDS::VertexIndex vertex_index_;
	RDS::SparseVariableIndex sparse_variable_index_;
	CoordinateType coordinate_type_;
	double coordinate_value_;
};

#endif