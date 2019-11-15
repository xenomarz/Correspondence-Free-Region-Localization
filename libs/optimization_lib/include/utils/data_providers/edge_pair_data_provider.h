#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_EDGE_PAIR_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "./data_provider.h"
#include "../../utils/type_definitions.h"

class EdgePairDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::EdgePairDescriptor& edge_pair_descriptor);
	virtual ~EdgePairDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;

	/**
	 * Getters
	 */
	int64_t GetEdge1Vertex1XIndex() const;
	int64_t GetEdge1Vertex1YIndex() const;
	int64_t GetEdge1Vertex2XIndex() const;
	int64_t GetEdge1Vertex2YIndex() const;

	int64_t GetEdge2Vertex1XIndex() const;
	int64_t GetEdge2Vertex1YIndex() const;
	int64_t GetEdge2Vertex2XIndex() const;
	int64_t GetEdge2Vertex2YIndex() const;

	double GetEdge1XDiff() const;
	double GetEdge1YDiff() const;
	double GetEdge2XDiff() const;
	double GetEdge2YDiff() const;

	double GetEdge1XSquaredDiff() const;
	double GetEdge1YSquaredDiff() const;
	double GetEdge2XSquaredDiff() const;
	double GetEdge2YSquaredDiff() const;

	double GetEdge1SquaredNrom() const;
	double GetEdge2SquaredNrom() const;

	double GetEdge1QuadrupledNrom() const;
	double GetEdge2QuadrupledNrom() const;

private:
	RDS::EdgePairDescriptor edge_pair_descriptor_;
	
	int64_t edge1_v1_x_index_;
	int64_t edge1_v1_y_index_;
	int64_t edge1_v2_x_index_;
	int64_t edge1_v2_y_index_;

	int64_t edge2_v1_x_index_;
	int64_t edge2_v1_y_index_;
	int64_t edge2_v2_x_index_;
	int64_t edge2_v2_y_index_;

	double edge1_x_diff_;
	double edge1_y_diff_;
	double edge2_x_diff_;
	double edge2_y_diff_;

	double edge1_x_diff_squared_;
	double edge1_y_diff_squared_;
	double edge2_x_diff_squared_;
	double edge2_y_diff_squared_;

	double edge1_squared_norm_;
	double edge2_squared_norm_;

	double edge1_quadrupled_norm_;
	double edge2_quadrupled_norm_;
};

#endif