#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_LENGTH_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_LENGTH_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// Optimization lib includes
#include "../../data_providers/edge_pair_data_provider.h"
#include "./edge_pair_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class EdgePairLengthObjective : public EdgePairObjective<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairLengthObjective(const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider) :
		EdgePairObjective(edge_pair_data_provider, "Edge Pair Length Objective")
	{
		this->Initialize();
	}

	virtual ~EdgePairLengthObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		auto& edge_pair_data_provider = this->GetEdgePairDataProvider();

		auto e1_v1_x_index = edge_pair_data_provider.GetEdge1Vertex1XIndex();
		auto e1_v1_y_index = edge_pair_data_provider.GetEdge1Vertex1YIndex();
		auto e1_v2_x_index = edge_pair_data_provider.GetEdge1Vertex2XIndex();
		auto e1_v2_y_index = edge_pair_data_provider.GetEdge1Vertex2YIndex();
		auto e2_v1_x_index = edge_pair_data_provider.GetEdge2Vertex1XIndex();
		auto e2_v1_y_index = edge_pair_data_provider.GetEdge2Vertex1YIndex();
		auto e2_v2_x_index = edge_pair_data_provider.GetEdge2Vertex2XIndex();
		auto e2_v2_y_index = edge_pair_data_provider.GetEdge2Vertex2YIndex();
		
		auto e1_y_diff = edge_pair_data_provider.GetEdge1YDiff();
		auto e1_x_diff = edge_pair_data_provider.GetEdge1XDiff();
		auto e2_y_diff = edge_pair_data_provider.GetEdge2YDiff();
		auto e2_x_diff = edge_pair_data_provider.GetEdge2XDiff();

		auto e1_squared_norm = edge_pair_data_provider.GetEdge1SquaredNrom();
		auto e2_squared_norm = edge_pair_data_provider.GetEdge2SquaredNrom();

		squared_norm_diff_ = e1_squared_norm - e2_squared_norm;
		
		auto squared_norm_diff_scaled = 4 * squared_norm_diff_;

		auto e1_y_diff_scaled = e1_y_diff * squared_norm_diff_scaled;
		auto e1_x_diff_scaled = e1_x_diff * squared_norm_diff_scaled;
		auto e2_y_diff_scaled = e2_y_diff * squared_norm_diff_scaled;
		auto e2_x_diff_scaled = e2_x_diff * squared_norm_diff_scaled;

		/**
		 * First partial derivatives
		 */
		this->sparse_index_to_first_derivative_value_map_[e1_v1_x_index] = e1_x_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e1_v1_y_index] = e1_y_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e1_v2_x_index] = e1_x_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e1_v2_y_index] = e1_y_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e2_v1_x_index] = e2_x_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e2_v1_y_index] = e2_y_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e2_v2_x_index] = e2_x_diff_scaled;
		this->sparse_index_to_first_derivative_value_map_[e2_v2_y_index] = e2_y_diff_scaled;

		/**
		 * Second partial derivatives
		 */
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v1_x_index }] = e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v1_y_index }] = -e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v2_x_index }] = -e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v2_y_index }] = e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v2_y_index }] = 0;

		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v1_x_index }] = e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v1_y_index }] = -e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v2_x_index }] = -e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v2_y_index }] = e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v2_y_index }] = 0;

		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v1_x_index }] = e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v1_y_index }] = e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v2_x_index }] = -e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v2_y_index }] = -e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v2_y_index }] = 0;

		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v1_x_index }] = e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v1_y_index }] = e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v2_x_index }] = -e1_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v2_y_index }] = -e1_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v2_y_index }] = 0;

		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v2_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v1_x_index }] = e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v1_y_index }] = -e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v2_x_index }] = -e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v2_y_index }] = e2_squares_diff_prod_to_quad_norm;

		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v2_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v1_x_index }] = e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v1_y_index }] = -e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v2_x_index }] = -e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v2_y_index }] = e2_squares_diff_prod_to_quad_norm;

		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v2_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v1_x_index }] = e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v1_y_index }] = e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v2_x_index }] = -e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v2_y_index }] = -e2_diff_prod_to_quad_norm;

		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v1_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v1_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v2_x_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v2_y_index }] = 0;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v1_x_index }] = e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v1_y_index }] = e2_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v2_x_index }] = -e2_squares_diff_prod_to_quad_norm;
		//this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v2_y_index }] = -e2_diff_prod_to_quad_norm;
	}
	
private:
	/**
	 * Private overrides
	 */
	void InitializeSparseIndexToFirstDerivativeSignMap(std::unordered_map<RDS::SparseVariableIndex, double>& sparse_index_to_first_derivative_sign_map) override
	{
		auto& edge_pair_data_provider = this->GetEdgePairDataProvider();
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge1Vertex1XIndex(), -1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge1Vertex1YIndex(), -1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge1Vertex2XIndex(),  1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge1Vertex2YIndex(),  1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge2Vertex1XIndex(),  1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge2Vertex1YIndex(),  1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge2Vertex2XIndex(), -1 });
		sparse_index_to_first_derivative_sign_map.insert({ edge_pair_data_provider.GetEdge2Vertex2YIndex(), -1 });
	}
	
	void CalculateValue(double& f) override
	{
		f = squared_norm_diff_ * squared_norm_diff_;
	}

	/**
	 * Private fields
	 */
	double squared_norm_diff_;
};

#endif