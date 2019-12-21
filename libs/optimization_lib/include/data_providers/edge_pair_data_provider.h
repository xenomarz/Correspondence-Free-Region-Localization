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
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) override;

	/**
	 * Getters
	 */
	inline const Eigen::Vector2d& EdgePairDataProvider::GetEdge1() const
	{
		return edge1_;
	}

	inline const Eigen::Vector2d& EdgePairDataProvider::GetEdge2() const
	{
		return edge2_;
	}

	inline int64_t EdgePairDataProvider::GetEdge1Vertex1XIndex() const
	{
		return edge1_v1_x_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge1Vertex1YIndex() const
	{
		return edge1_v1_y_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge1Vertex2XIndex() const
	{
		return edge1_v2_x_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge1Vertex2YIndex() const
	{
		return edge1_v2_y_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge2Vertex1XIndex() const
	{
		return edge2_v1_x_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge2Vertex1YIndex() const
	{
		return edge2_v1_y_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge2Vertex2XIndex() const
	{
		return edge2_v2_x_index_;
	}

	inline int64_t EdgePairDataProvider::GetEdge2Vertex2YIndex() const
	{
		return edge2_v2_y_index_;
	}

	inline double EdgePairDataProvider::GetEdge1XDiff() const
	{
		return edge1_x_diff_;
	}

	inline double EdgePairDataProvider::GetEdge1YDiff() const
	{
		return edge1_y_diff_;
	}

	inline double EdgePairDataProvider::GetEdge2XDiff() const
	{
		return edge2_x_diff_;
	}

	inline double EdgePairDataProvider::GetEdge2YDiff() const
	{
		return edge2_y_diff_;
	}

	inline double EdgePairDataProvider::GetEdge1XSquaredDiff() const
	{
		return edge1_x_diff_squared_;
	}

	inline double EdgePairDataProvider::GetEdge1YSquaredDiff() const
	{
		return edge1_y_diff_squared_;
	}

	inline double EdgePairDataProvider::GetEdge2XSquaredDiff() const
	{
		return edge2_x_diff_squared_;
	}

	inline double EdgePairDataProvider::GetEdge2YSquaredDiff() const
	{
		return edge2_y_diff_squared_;
	}

	inline double EdgePairDataProvider::GetEdge1SquaredNrom() const
	{
		return edge1_squared_norm_;
	}

	inline double EdgePairDataProvider::GetEdge2SquaredNrom() const
	{
		return edge2_squared_norm_;
	}

	inline double EdgePairDataProvider::GetEdge1QuadrupledNrom() const
	{
		return edge1_quadrupled_norm_;
	}

	inline double EdgePairDataProvider::GetEdge2QuadrupledNrom() const
	{
		return edge2_quadrupled_norm_;
	}

private:
	RDS::EdgePairDescriptor edge_pair_descriptor_;

	Eigen::Vector2d edge1_;
	Eigen::Vector2d edge2_;
	
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