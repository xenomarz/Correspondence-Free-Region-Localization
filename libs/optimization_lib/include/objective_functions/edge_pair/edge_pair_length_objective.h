#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_LENGTH_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_LENGTH_OBJECTIVE_H

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
	EdgePairLengthObjective(const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider, const bool enforce_psd = false) :
		EdgePairObjective(edge_pair_data_provider, "Edge Pair Length Objective", enforce_psd)
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
	void PreUpdate(const Eigen::VectorXd& x, UpdatableObject::UpdatedObjectSet& updated_objects) override
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

		auto e1_squared_norm = edge_pair_data_provider.GetEdge1SquaredNrom();
		auto e2_squared_norm = edge_pair_data_provider.GetEdge2SquaredNrom();

		squared_norm_diff_ = e1_squared_norm - e2_squared_norm;
		
		auto squared_norm_diff_scaled = 4 * squared_norm_diff_;

		Eigen::Vector2d edge1 = edge_pair_data_provider.GetEdge1();
		Eigen::Vector2d edge2 = edge_pair_data_provider.GetEdge2();

		Eigen::Vector2d edge1_scaled = squared_norm_diff_scaled * edge1;
		Eigen::Vector2d edge2_scaled = squared_norm_diff_scaled * edge2;

		Eigen::Matrix2d I = Eigen::Matrix2d::Identity();
		Eigen::Matrix2d I_scaled = squared_norm_diff_scaled * I;

		Eigen::Matrix2d d_edge1_d_edge1 = 8 * edge1 * edge1.transpose() + I_scaled;
		Eigen::Matrix2d d_edge1_v1_d_edge1_v1 = -d_edge1_d_edge1;
		Eigen::Matrix2d d_edge1_v1_d_edge1_v2 = d_edge1_d_edge1;
		Eigen::Matrix2d d_edge1_v2_d_edge1_v1 = -d_edge1_d_edge1;
		Eigen::Matrix2d d_edge1_v2_d_edge1_v2 = d_edge1_d_edge1;

		Eigen::Matrix2d d_edge1_d_edge2 = 8 * edge1 * edge2.transpose();
		Eigen::Matrix2d d_edge1_v1_d_edge2_v1 = d_edge1_d_edge2;
		Eigen::Matrix2d d_edge1_v1_d_edge2_v2 = -d_edge1_d_edge2;
		Eigen::Matrix2d d_edge1_v2_d_edge2_v1 = d_edge1_d_edge2;
		Eigen::Matrix2d d_edge1_v2_d_edge2_v2 = -d_edge1_d_edge2;

		Eigen::Matrix2d d_edge2_d_edge1 = 8 * edge2 * edge1.transpose();
		Eigen::Matrix2d d_edge2_v1_d_edge1_v1 = -d_edge2_d_edge1;
		Eigen::Matrix2d d_edge2_v1_d_edge1_v2 = d_edge2_d_edge1;
		Eigen::Matrix2d d_edge2_v2_d_edge1_v1 = -d_edge2_d_edge1;
		Eigen::Matrix2d d_edge2_v2_d_edge1_v2 = d_edge2_d_edge1;

		Eigen::Matrix2d d_edge2_d_edge2 = -8 * edge2 * edge2.transpose() + I_scaled;
		Eigen::Matrix2d d_edge2_v1_d_edge2_v1 = -d_edge2_d_edge2;
		Eigen::Matrix2d d_edge2_v1_d_edge2_v2 = d_edge2_d_edge2;
		Eigen::Matrix2d d_edge2_v2_d_edge2_v1 = -d_edge2_d_edge2;
		Eigen::Matrix2d d_edge2_v2_d_edge2_v2 = d_edge2_d_edge2;

		/**
		 * First partial derivatives
		 */
		this->sparse_index_to_first_derivative_value_map_[e1_v1_x_index] = edge1_scaled.coeff(0);
		this->sparse_index_to_first_derivative_value_map_[e1_v1_y_index] = edge1_scaled.coeff(1);
		this->sparse_index_to_first_derivative_value_map_[e1_v2_x_index] = edge1_scaled.coeff(0);
		this->sparse_index_to_first_derivative_value_map_[e1_v2_y_index] = edge1_scaled.coeff(1);
		this->sparse_index_to_first_derivative_value_map_[e2_v1_x_index] = edge2_scaled.coeff(0);
		this->sparse_index_to_first_derivative_value_map_[e2_v1_y_index] = edge2_scaled.coeff(1);
		this->sparse_index_to_first_derivative_value_map_[e2_v2_x_index] = edge2_scaled.coeff(0);
		this->sparse_index_to_first_derivative_value_map_[e2_v2_y_index] = edge2_scaled.coeff(1);

		/**
		 * Second partial derivatives
		 */
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v1_x_index }] = d_edge1_v1_d_edge1_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v1_y_index }] = d_edge1_v1_d_edge1_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v2_x_index }] = d_edge1_v1_d_edge1_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e1_v2_y_index }] = d_edge1_v1_d_edge1_v2(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v1_x_index }] = d_edge1_v1_d_edge2_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v1_y_index }] = d_edge1_v1_d_edge2_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v2_x_index }] = d_edge1_v1_d_edge2_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_x_index, e2_v2_y_index }] = d_edge1_v1_d_edge2_v2(0, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v1_x_index }] = d_edge1_v2_d_edge1_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v1_y_index }] = d_edge1_v2_d_edge1_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v2_x_index }] = d_edge1_v2_d_edge1_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e1_v2_y_index }] = d_edge1_v2_d_edge1_v2(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v1_x_index }] = d_edge1_v2_d_edge2_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v1_y_index }] = d_edge1_v2_d_edge2_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v2_x_index }] = d_edge1_v2_d_edge2_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_x_index, e2_v2_y_index }] = d_edge1_v2_d_edge2_v2(0, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v1_x_index }] = d_edge1_v1_d_edge1_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v1_y_index }] = d_edge1_v1_d_edge1_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v2_x_index }] = d_edge1_v1_d_edge1_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e1_v2_y_index }] = d_edge1_v1_d_edge1_v2(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v1_x_index }] = d_edge1_v1_d_edge2_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v1_y_index }] = d_edge1_v1_d_edge2_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v2_x_index }] = d_edge1_v1_d_edge2_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v1_y_index, e2_v2_y_index }] = d_edge1_v1_d_edge2_v2(1, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v1_x_index }] = d_edge1_v2_d_edge1_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v1_y_index }] = d_edge1_v2_d_edge1_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v2_x_index }] = d_edge1_v2_d_edge1_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e1_v2_y_index }] = d_edge1_v2_d_edge1_v2(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v1_x_index }] = d_edge1_v2_d_edge2_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v1_y_index }] = d_edge1_v2_d_edge2_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v2_x_index }] = d_edge1_v2_d_edge2_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e1_v2_y_index, e2_v2_y_index }] = d_edge1_v2_d_edge2_v2(1, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v1_x_index }] = d_edge2_v1_d_edge1_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v1_y_index }] = d_edge2_v1_d_edge1_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v2_x_index }] = d_edge2_v1_d_edge1_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e1_v2_y_index }] = d_edge2_v1_d_edge1_v2(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v1_x_index }] = d_edge2_v1_d_edge2_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v1_y_index }] = d_edge2_v1_d_edge2_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v2_x_index }] = d_edge2_v1_d_edge2_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_x_index, e2_v2_y_index }] = d_edge2_v1_d_edge2_v2(0, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v1_x_index }] = d_edge2_v2_d_edge1_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v1_y_index }] = d_edge2_v2_d_edge1_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v2_x_index }] = d_edge2_v2_d_edge1_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e1_v2_y_index }] = d_edge2_v2_d_edge1_v2(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v1_x_index }] = d_edge2_v2_d_edge2_v1(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v1_y_index }] = d_edge2_v2_d_edge2_v1(0, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v2_x_index }] = d_edge2_v2_d_edge2_v2(0, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_x_index, e2_v2_y_index }] = d_edge2_v2_d_edge2_v2(0, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v1_x_index }] = d_edge2_v1_d_edge1_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v1_y_index }] = d_edge2_v1_d_edge1_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v2_x_index }] = d_edge2_v1_d_edge1_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e1_v2_y_index }] = d_edge2_v1_d_edge1_v2(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v1_x_index }] = d_edge2_v1_d_edge2_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v1_y_index }] = d_edge2_v1_d_edge2_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v2_x_index }] = d_edge2_v1_d_edge2_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v1_y_index, e2_v2_y_index }] = d_edge2_v1_d_edge2_v2(1, 1);

		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v1_x_index }] = d_edge2_v2_d_edge1_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v1_y_index }] = d_edge2_v2_d_edge1_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v2_x_index }] = d_edge2_v2_d_edge1_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e1_v2_y_index }] = d_edge2_v2_d_edge1_v2(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v1_x_index }] = d_edge2_v2_d_edge2_v1(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v1_y_index }] = d_edge2_v2_d_edge2_v1(1, 1);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v2_x_index }] = d_edge2_v2_d_edge2_v2(1, 0);
		this->sparse_indices_to_second_derivative_value_map_[{ e2_v2_y_index, e2_v2_y_index }] = d_edge2_v2_d_edge2_v2(1, 1);
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