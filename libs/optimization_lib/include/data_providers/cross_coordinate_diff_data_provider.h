#pragma once
#ifndef OPTIMIZATION_LIB_CROSS_COORDINATE_DIFF_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_CROSS_COORDINATE_DIFF_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"

class CrossCoordinateDiffDataProvider : public DataProvider
{
public:	
	/**
	 * Constructors and destructor
	 */
	CrossCoordinateDiffDataProvider(
		const std::shared_ptr<MeshDataProvider>& mesh_data_provider,
		const RDS::VertexIndex edge1_vertex1_index,
		const RDS::VertexIndex edge1_vertex2_index,
		const RDS::VertexIndex edge2_vertex1_index,
		const RDS::VertexIndex edge2_vertex2_index,
		const RDS::CoordinateType coordinate_type);
	virtual ~CrossCoordinateDiffDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void Update(const Eigen::VectorXd& x, const int32_t update_modifiers) override;
	double GetCoordinate1DiffValue() const;
	double GetCoordinate2DiffValue() const;
	double GetCrossCoordinateDiffValue() const;
	double GetCrossCoordinateDiffValueSquared() const;
	RDS::SparseVariableIndex GetEdge1Variable1Index() const;
	RDS::SparseVariableIndex GetEdge1Variable2Index() const;
	RDS::SparseVariableIndex GetEdge2Variable1Index() const;
	RDS::SparseVariableIndex GetEdge2Variable2Index() const;
	
private:
	/**
	 * Private fields
	 */
	RDS::VertexIndex edge1_vertex1_index_;
	RDS::VertexIndex edge1_vertex2_index_;
	RDS::VertexIndex edge2_vertex1_index_;
	RDS::VertexIndex edge2_vertex2_index_;
	RDS::SparseVariableIndex edge1_variable1_index_;
	RDS::SparseVariableIndex edge1_variable2_index_;
	RDS::SparseVariableIndex edge2_variable1_index_;
	RDS::SparseVariableIndex edge2_variable2_index_;
	RDS::CoordinateType coordinate_type_;
	double coordinate1_diff_value_;
	double coordinate2_diff_value_;
	double cross_coordinate_diff_value_;
	double cross_coordinate_diff_value_squared_;
};

#endif