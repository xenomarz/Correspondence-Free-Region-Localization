#pragma once
#ifndef OPTIMIZATION_LIB_COORDINATE_DIFF_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_COORDINATE_DIFF_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"

class CoordinateDiffDataProvider : public DataProvider
{
public:	
	/**
	 * Constructors and destructor
	 */
	CoordinateDiffDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex1_index, const RDS::VertexIndex vertex2_index, const RDS::CoordinateType coordinate_type);
	virtual ~CoordinateDiffDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, const int32_t update_modifiers) override;
	double GetCoordinateDiffValue() const;
	RDS::SparseVariableIndex GetSparseVariable1Index() const;
	RDS::SparseVariableIndex GetSparseVariable2Index() const;
	
private:
	/**
	 * Private fields
	 */
	RDS::VertexIndex vertex1_index_;
	RDS::VertexIndex vertex2_index_;
	RDS::SparseVariableIndex sparse_variable1_index_;
	RDS::SparseVariableIndex sparse_variable2_index_;
	RDS::CoordinateType coordinate_type_;
	double coordinate_diff_value_;
};

#endif