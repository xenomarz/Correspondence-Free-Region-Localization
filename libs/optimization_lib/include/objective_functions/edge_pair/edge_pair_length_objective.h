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
		this->dense_index_to_first_derivative_value_map_[this->e1_v1_x_dense_index_] = edge1_scaled.coeff(0);
		this->dense_index_to_first_derivative_value_map_[this->e1_v1_y_dense_index_] = edge1_scaled.coeff(1);
		this->dense_index_to_first_derivative_value_map_[this->e1_v2_x_dense_index_] = edge1_scaled.coeff(0);
		this->dense_index_to_first_derivative_value_map_[this->e1_v2_y_dense_index_] = edge1_scaled.coeff(1);
		this->dense_index_to_first_derivative_value_map_[this->e2_v1_x_dense_index_] = edge2_scaled.coeff(0);
		this->dense_index_to_first_derivative_value_map_[this->e2_v1_y_dense_index_] = edge2_scaled.coeff(1);
		this->dense_index_to_first_derivative_value_map_[this->e2_v2_x_dense_index_] = edge2_scaled.coeff(0);
		this->dense_index_to_first_derivative_value_map_[this->e2_v2_y_dense_index_] = edge2_scaled.coeff(1);

		/**
		 * Second partial derivatives
		 */
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v1_x_dense_index_] = d_edge1_v1_d_edge1_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v1_y_dense_index_] = d_edge1_v1_d_edge1_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v2_x_dense_index_] = d_edge1_v1_d_edge1_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v2_y_dense_index_] = d_edge1_v1_d_edge1_v2(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v1_x_dense_index_] = d_edge1_v1_d_edge2_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v1_y_dense_index_] = d_edge1_v1_d_edge2_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v2_x_dense_index_] = d_edge1_v1_d_edge2_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v2_y_dense_index_] = d_edge1_v1_d_edge2_v2(0, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v1_x_dense_index_] = d_edge1_v2_d_edge1_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v1_y_dense_index_] = d_edge1_v2_d_edge1_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v2_x_dense_index_] = d_edge1_v2_d_edge1_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v2_y_dense_index_] = d_edge1_v2_d_edge1_v2(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v1_x_dense_index_] = d_edge1_v2_d_edge2_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v1_y_dense_index_] = d_edge1_v2_d_edge2_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v2_x_dense_index_] = d_edge1_v2_d_edge2_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v2_y_dense_index_] = d_edge1_v2_d_edge2_v2(0, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v1_x_dense_index_] = d_edge1_v1_d_edge1_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v1_y_dense_index_] = d_edge1_v1_d_edge1_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v2_x_dense_index_] = d_edge1_v1_d_edge1_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v2_y_dense_index_] = d_edge1_v1_d_edge1_v2(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v1_x_dense_index_] = d_edge1_v1_d_edge2_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v1_y_dense_index_] = d_edge1_v1_d_edge2_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v2_x_dense_index_] = d_edge1_v1_d_edge2_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v2_y_dense_index_] = d_edge1_v1_d_edge2_v2(1, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v1_x_dense_index_] = d_edge1_v2_d_edge1_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v1_y_dense_index_] = d_edge1_v2_d_edge1_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v2_x_dense_index_] = d_edge1_v2_d_edge1_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v2_y_dense_index_] = d_edge1_v2_d_edge1_v2(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v1_x_dense_index_] = d_edge1_v2_d_edge2_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v1_y_dense_index_] = d_edge1_v2_d_edge2_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v2_x_dense_index_] = d_edge1_v2_d_edge2_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v2_y_dense_index_] = d_edge1_v2_d_edge2_v2(1, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v1_x_dense_index_] = d_edge2_v1_d_edge1_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v1_y_dense_index_] = d_edge2_v1_d_edge1_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v2_x_dense_index_] = d_edge2_v1_d_edge1_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v2_y_dense_index_] = d_edge2_v1_d_edge1_v2(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v1_x_dense_index_] = d_edge2_v1_d_edge2_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v1_y_dense_index_] = d_edge2_v1_d_edge2_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v2_x_dense_index_] = d_edge2_v1_d_edge2_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v2_y_dense_index_] = d_edge2_v1_d_edge2_v2(0, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v1_x_dense_index_] = d_edge2_v2_d_edge1_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v1_y_dense_index_] = d_edge2_v2_d_edge1_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v2_x_dense_index_] = d_edge2_v2_d_edge1_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v2_y_dense_index_] = d_edge2_v2_d_edge1_v2(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v1_x_dense_index_] = d_edge2_v2_d_edge2_v1(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v1_y_dense_index_] = d_edge2_v2_d_edge2_v1(0, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v2_x_dense_index_] = d_edge2_v2_d_edge2_v2(0, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v2_y_dense_index_] = d_edge2_v2_d_edge2_v2(0, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v1_x_dense_index_] = d_edge2_v1_d_edge1_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v1_y_dense_index_] = d_edge2_v1_d_edge1_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v2_x_dense_index_] = d_edge2_v1_d_edge1_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v2_y_dense_index_] = d_edge2_v1_d_edge1_v2(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v1_x_dense_index_] = d_edge2_v1_d_edge2_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v1_y_dense_index_] = d_edge2_v1_d_edge2_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v2_x_dense_index_] = d_edge2_v1_d_edge2_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v2_y_dense_index_] = d_edge2_v1_d_edge2_v2(1, 1);

		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v1_x_dense_index_] = d_edge2_v2_d_edge1_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v1_y_dense_index_] = d_edge2_v2_d_edge1_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v2_x_dense_index_] = d_edge2_v2_d_edge1_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v2_y_dense_index_] = d_edge2_v2_d_edge1_v2(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v1_x_dense_index_] = d_edge2_v2_d_edge2_v1(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v1_y_dense_index_] = d_edge2_v2_d_edge2_v1(1, 1);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v2_x_dense_index_] = d_edge2_v2_d_edge2_v2(1, 0);
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v2_y_dense_index_] = d_edge2_v2_d_edge2_v2(1, 1);
	}
	
private:
	/**
	 * Private overrides
	 */
	void InitializeDenseIndexToFirstDerivativeSignMap(std::vector<double>& dense_index_to_first_derivative_sign_map) override
	{
		dense_index_to_first_derivative_sign_map[this->e1_v1_x_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e1_v1_y_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e1_v2_x_dense_index_] =  1;
		dense_index_to_first_derivative_sign_map[this->e1_v2_y_dense_index_] =  1;
		dense_index_to_first_derivative_sign_map[this->e2_v1_x_dense_index_] =  1;
		dense_index_to_first_derivative_sign_map[this->e2_v1_y_dense_index_] =  1;
		dense_index_to_first_derivative_sign_map[this->e2_v2_x_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e2_v2_y_dense_index_] = -1;
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