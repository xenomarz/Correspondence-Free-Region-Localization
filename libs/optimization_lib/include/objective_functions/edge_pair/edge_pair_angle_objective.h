#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// STL includes
#include <utility>
#include <cmath>
#include <algorithm>
#include <unordered_map>

// Eigen includes
#include <Eigen/Eigenvalues> 

// Optimization lib includes
#include "../../utils/type_definitions.h"
#include "./edge_pair_objective.h"


template<Eigen::StorageOptions StorageOrder_>
class EdgePairAngleObjective : public EdgePairObjective<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairAngleObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::EdgePairDescriptor& edge_pair_descriptor) :
		EdgePairObjective(mesh_data_provider, "Edge Pair Angle Objective", edge_pair_descriptor)
	{
		this->Initialize();
	}

	virtual ~EdgePairAngleObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		edge1_v1_ = Eigen::Vector2d(x(edge1_v1_x_index_), x(edge1_v1_y_index_));
		edge1_v2_ = Eigen::Vector2d(x(edge1_v2_x_index_), x(edge1_v2_y_index_));
		
		edge2_v1_ = Eigen::Vector2d(x(edge2_v1_x_index_), x(edge2_v1_y_index_));
		edge2_v2_ = Eigen::Vector2d(x(edge2_v2_x_index_), x(edge2_v2_y_index_));

		edge1_x_diff_ = edge1_v2_.x() - edge1_v1_.x();
		edge1_y_diff_ = edge1_v2_.y() - edge1_v1_.y();
		edge2_x_diff_ = edge2_v2_.x() - edge2_v1_.x();
		edge2_y_diff_ = edge2_v2_.y() - edge2_v1_.y();

		edge1_x_diff_squared_ = edge1_x_diff_ * edge1_x_diff_;
		edge1_y_diff_squared_ = edge1_y_diff_ * edge1_y_diff_;
		edge2_x_diff_squared_ = edge2_x_diff_ * edge2_x_diff_;
		edge2_y_diff_squared_ = edge2_y_diff_ * edge2_y_diff_;

		edge1_squared_norm_ = edge1_x_diff_squared_ + edge1_y_diff_squared_;
		edge2_squared_norm_ = edge2_x_diff_squared_ + edge2_y_diff_squared_;
		
		edge1_quadrupled_norm_ = edge1_squared_norm_ * edge1_squared_norm_;
		edge2_quadrupled_norm_ = edge2_squared_norm_ * edge2_squared_norm_;

		double e1_y_to_e1_squared_norm = edge1_y_diff_ / edge1_squared_norm_;
		double e1_x_to_e1_squared_norm = edge1_x_diff_ / edge1_squared_norm_;
		double e2_y_to_e2_squared_norm = edge2_y_diff_ / edge2_squared_norm_;
		double e2_x_to_e2_squared_norm = edge2_x_diff_ / edge2_squared_norm_;

		partial_to_first_derivative_value_map_[Partial::E1_V1_X] = e1_y_to_e1_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E1_V1_Y] = e1_x_to_e1_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E1_V2_X] = e1_y_to_e1_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E1_V2_Y] = e1_x_to_e1_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E2_V1_X] = e2_y_to_e2_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E2_V1_Y] = e2_x_to_e2_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E2_V2_X] = e2_y_to_e2_squared_norm;
		partial_to_first_derivative_value_map_[Partial::E2_V2_Y] = e2_x_to_e2_squared_norm;

		double e1_diff_prod_to_quad_norm = (2 * edge1_x_diff_ * edge1_y_diff_) / edge1_quadrupled_norm_;
		double e2_diff_prod_to_quad_norm = (2 * edge2_x_diff_ * edge2_y_diff_) / edge2_quadrupled_norm_;
		double e1_squares_diff_prod_to_quad_norm = (edge1_x_diff_squared_ - edge1_y_diff_squared_) / edge1_quadrupled_norm_;
		double e2_squares_diff_prod_to_quad_norm = (edge2_x_diff_squared_ - edge2_y_diff_squared_) / edge2_quadrupled_norm_;

		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E1_V1_X }] = e1_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E1_V1_Y }] = -e1_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E1_V2_X }] = -e1_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E1_V2_Y }] = e1_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E2_V1_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E2_V1_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E2_V2_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_X, Partial::E2_V2_Y }] = 0;

		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E1_V1_X }] = e1_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E1_V1_Y }] = e1_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E1_V2_X }] = -e1_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E1_V2_Y }] = -e1_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E2_V1_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E2_V1_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E2_V2_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E1_Y, Partial::E2_V2_Y }] = 0;

		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E1_V1_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E1_V1_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E1_V2_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E1_V2_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E2_V1_X }] = e2_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E2_V1_Y }] = -e2_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E2_V2_X }] = -e2_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_X, Partial::E2_V2_Y }] = e2_squares_diff_prod_to_quad_norm;

		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E1_V1_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E1_V1_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E1_V2_X }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E1_V2_Y }] = 0;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E2_V1_X }] = e2_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E2_V1_Y }] = e2_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E2_V2_X }] = -e2_squares_diff_prod_to_quad_norm;
		partial_to_second_derivative_value_map_[{ PartialCoordinateType::E2_Y, Partial::E2_V2_Y }] = -e2_diff_prod_to_quad_norm;
	}

private:
	/**
	 * Private overrides
	 */
	void CalculateValue(double& f) override
	{
		//f = std::atan2(edge1_y_diff_, edge1_x_diff_) - atan2(edge2_y_diff_, edge2_x_diff_);
	}
	
	/**
	 * Private fields
	 */
	double edge1_x_diff_;
	double edge1_y_diff_;
	double edge2_x_diff_;
	double edge2_y_diff_;

	double edge1_x_diff_squared_;
	double edge1_y_diff_squared_;
	double edge2_x_diff_squared_;
	double edge2_y_diff_squared_;
	
	double edge1_atan2_dx_;
	double edge1_atan2_dy_;
	double edge2_atan2_dx_;
	double edge2_atan2_dy_;

	double edge1_squared_norm_;
	double edge2_squared_norm_;

	double edge1_quadrupled_norm_;
	double edge2_quadrupled_norm_;
};

#endif