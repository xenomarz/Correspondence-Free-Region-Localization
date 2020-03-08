#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_EDGE_PAIR_DATA_PROVIDER_H

// STL includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../core/core.h"
#include "./data_provider.h"

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
	void Update(const Eigen::VectorXd& x, int32_t update_modifiers) override;

	/**
	 * Getters
	 */
	const Eigen::Vector2d& EdgePairDataProvider::GetEdge1() const
	{
		return edge1_;
	}

	const Eigen::Vector2d& EdgePairDataProvider::GetEdge2() const
	{
		return edge2_;
	}

	RDS::EdgeIndex EdgePairDataProvider::GetImageEdge1Index() const
	{
		return image_edge_1_index_;
	}
	
	RDS::EdgeIndex EdgePairDataProvider::GetImageEdge2Index() const
	{
		return image_edge_2_index_;
	}
	
	RDS::EdgeIndex EdgePairDataProvider::GetDomainEdgeIndex() const
	{
		return domain_edge_index_;
	}

	RDS::VertexIndex EdgePairDataProvider::GetEdge1Vertex1Index() const
	{
		return edge1_v1_index_;
	}

	RDS::VertexIndex EdgePairDataProvider::GetEdge1Vertex2Index() const
	{
		return edge1_v2_index_;
	}

	RDS::VertexIndex EdgePairDataProvider::GetEdge2Vertex1Index() const
	{
		return edge2_v1_index_;
	}

	RDS::VertexIndex EdgePairDataProvider::GetEdge2Vertex2Index() const
	{
		return edge2_v2_index_;
	}

	int64_t EdgePairDataProvider::GetEdge1Vertex1XIndex() const
	{
		return edge1_v1_x_index_;
	}

	int64_t EdgePairDataProvider::GetEdge1Vertex1YIndex() const
	{
		return edge1_v1_y_index_;
	}

	int64_t EdgePairDataProvider::GetEdge1Vertex2XIndex() const
	{
		return edge1_v2_x_index_;
	}

	int64_t EdgePairDataProvider::GetEdge1Vertex2YIndex() const
	{
		return edge1_v2_y_index_;
	}

	int64_t EdgePairDataProvider::GetEdge2Vertex1XIndex() const
	{
		return edge2_v1_x_index_;
	}

	int64_t EdgePairDataProvider::GetEdge2Vertex1YIndex() const
	{
		return edge2_v1_y_index_;
	}

	int64_t EdgePairDataProvider::GetEdge2Vertex2XIndex() const
	{
		return edge2_v2_x_index_;
	}

	int64_t EdgePairDataProvider::GetEdge2Vertex2YIndex() const
	{
		return edge2_v2_y_index_;
	}

	double EdgePairDataProvider::GetEdge1XDiff() const
	{
		return edge1_x_diff_;
	}

	double EdgePairDataProvider::GetEdge1YDiff() const
	{
		return edge1_y_diff_;
	}

	double EdgePairDataProvider::GetEdge2XDiff() const
	{
		return edge2_x_diff_;
	}

	double EdgePairDataProvider::GetEdge2YDiff() const
	{
		return edge2_y_diff_;
	}

	double EdgePairDataProvider::GetEdge1XSquaredDiff() const
	{
		return edge1_x_diff_squared_;
	}

	double EdgePairDataProvider::GetEdge1YSquaredDiff() const
	{
		return edge1_y_diff_squared_;
	}

	double EdgePairDataProvider::GetEdge2XSquaredDiff() const
	{
		return edge2_x_diff_squared_;
	}

	double EdgePairDataProvider::GetEdge2YSquaredDiff() const
	{
		return edge2_y_diff_squared_;
	}

	double EdgePairDataProvider::GetEdge1SquaredNrom() const
	{
		return edge1_squared_norm_;
	}

	double EdgePairDataProvider::GetEdge2SquaredNrom() const
	{
		return edge2_squared_norm_;
	}

	double EdgePairDataProvider::GetEdge1QuadrupledNrom() const
	{
		return edge1_quadrupled_norm_;
	}

	double EdgePairDataProvider::GetEdge2QuadrupledNrom() const
	{
		return edge2_quadrupled_norm_;
	}

	double EdgePairDataProvider::GetVertex1XDiff() const
	{
		return vertex1_x_diff_;
	}

	double EdgePairDataProvider::GetVertex1YDiff() const
	{
		return vertex1_y_diff_;
	}

	double EdgePairDataProvider::GetVertex2XDiff() const
	{
		return vertex2_x_diff_;
	}

	double EdgePairDataProvider::GetVertex2YDiff() const
	{
		return vertex2_y_diff_;
	}
	
private:
	RDS::EdgePairDescriptor edge_pair_descriptor_;
	RDS::EdgeIndex image_edge_1_index_;
	RDS::EdgeIndex image_edge_2_index_;
	RDS::EdgeIndex domain_edge_index_;
	
	Eigen::Vector2d edge1_;
	Eigen::Vector2d edge2_;

	RDS::VertexIndex edge1_v1_index_;
	RDS::VertexIndex edge1_v2_index_;
	RDS::VertexIndex edge2_v1_index_;
	RDS::VertexIndex edge2_v2_index_;

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

	double vertex1_x_diff_;
	double vertex1_y_diff_;
	double vertex2_x_diff_;
	double vertex2_y_diff_;
};

#endif