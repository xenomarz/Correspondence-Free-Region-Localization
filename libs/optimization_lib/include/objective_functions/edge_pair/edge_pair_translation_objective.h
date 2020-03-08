#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_TRANSLATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_TRANSLATION_OBJECTIVE_H

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../summation_objective.h"
#include "../../data_providers/edge_pair_data_provider.h"

template <Eigen::StorageOptions StorageOrder_>
class EdgePairTranslationObjective : public EdgePairObjective<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairTranslationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider, const bool enforce_psd = false) :
		EdgePairObjective(mesh_data_provider, edge_pair_data_provider, "Edge Pair Translation Objective", enforce_psd)
	{
		this->Initialize();
	}

	virtual ~EdgePairTranslationObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		auto& edge_pair_data_provider = this->GetEdgePairDataProvider();

		x_cross_diff_ = edge_pair_data_provider.GetVertex1XDiff() - edge_pair_data_provider.GetVertex2XDiff();
		y_cross_diff_ = edge_pair_data_provider.GetVertex1YDiff() - edge_pair_data_provider.GetVertex2YDiff();
		x_cross_diff_squared_ = x_cross_diff_ * x_cross_diff_;
		y_cross_diff_squared_ = y_cross_diff_ * y_cross_diff_;
		x_cross_diff_doubled_ = 2 * x_cross_diff_;
		y_cross_diff_doubled_ = 2 * y_cross_diff_;

		/**
		 * First partial derivatives
		 */
		this->dense_index_to_first_derivative_value_map_[this->e1_v1_x_dense_index_] = x_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e1_v1_y_dense_index_] = y_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e1_v2_x_dense_index_] = x_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e1_v2_y_dense_index_] = y_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e2_v1_x_dense_index_] = x_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e2_v1_y_dense_index_] = y_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e2_v2_x_dense_index_] = x_cross_diff_doubled_;
		this->dense_index_to_first_derivative_value_map_[this->e2_v2_y_dense_index_] = y_cross_diff_doubled_;

		/**
		 * Second partial derivatives
		 */
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v1_x_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e1_v2_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v1_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_x_dense_index_][this->e2_v2_x_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v1_x_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e1_v2_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v1_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_x_dense_index_][this->e2_v2_x_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v1_y_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e1_v2_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v1_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v1_y_dense_index_][this->e2_v2_y_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v1_y_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e1_v2_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v1_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e1_v2_y_dense_index_][this->e2_v2_y_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v1_x_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e1_v2_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v1_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_x_dense_index_][this->e2_v2_x_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v1_x_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e1_v2_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v1_x_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_x_dense_index_][this->e2_v2_x_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v1_y_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e1_v2_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v1_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v1_y_dense_index_][this->e2_v2_y_dense_index_] = 2;

		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v1_y_dense_index_] = 2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e1_v2_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v1_y_dense_index_] = -2;
		this->dense_indices_to_second_derivative_value_map_[this->e2_v2_y_dense_index_][this->e2_v2_y_dense_index_] = 2;
	}

private:
	/**
	 * Private overrides
	 */
	void InitializeDenseIndexToFirstDerivativeSignMap(std::vector<double>& dense_index_to_first_derivative_sign_map) override
	{
		dense_index_to_first_derivative_sign_map[this->e1_v1_x_dense_index_] = 1;
		dense_index_to_first_derivative_sign_map[this->e1_v1_y_dense_index_] = 1;
		dense_index_to_first_derivative_sign_map[this->e1_v2_x_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e1_v2_y_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e2_v1_x_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e2_v1_y_dense_index_] = -1;
		dense_index_to_first_derivative_sign_map[this->e2_v2_x_dense_index_] = 1;
		dense_index_to_first_derivative_sign_map[this->e2_v2_y_dense_index_] = 1;
	}

	void CalculateValue(double& f) override
	{
		auto& edge_pair_data_provider = this->GetEdgePairDataProvider();
		f = x_cross_diff_squared_ + y_cross_diff_squared_;
	}

	/**
	 * Private fields
	 */
	double x_cross_diff_;
	double y_cross_diff_;
	double x_cross_diff_squared_;
	double y_cross_diff_squared_;
	double x_cross_diff_doubled_;
	double y_cross_diff_doubled_;
};

#endif