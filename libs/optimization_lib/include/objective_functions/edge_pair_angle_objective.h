#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H

// STL includes
#include <utility>
#include <cmath>

// Optimization lib includes
#include "./periodic_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class EdgePairAngleObjective : public PeriodicObjective<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : uint32_t
	{
		Period = SparseObjectiveFunction<StorageOrder_>::Properties::Count_
	};

	/**
	 * Constructors and destructor
	 */
	EdgePairAngleObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::pair<uint64_t, uint64_t>& edge1_indices, const std::pair<uint64_t, uint64_t>& edge2_indices) :
		PeriodicObjective(objective_function_data_provider, "Edge Pair Angle Objective"),
		edge1_indices_(edge1_indices),
		edge2_indices_(edge2_indices)
	{
		this->Initialize();

		uint64_t edge1_v1_x_index = edge1_indices_.first;
		uint64_t edge1_v1_y_index = edge1_indices_.first + this->image_vertices_count_;
		uint64_t edge1_v2_x_index = edge1_indices_.second;
		uint64_t edge1_v2_y_index = edge1_indices_.second + this->image_vertices_count_;

		uint64_t edge2_v1_x_index = edge2_indices_.first;
		uint64_t edge2_v1_y_index = edge2_indices_.first + this->image_vertices_count_;
		uint64_t edge2_v2_x_index = edge2_indices_.second;
		uint64_t edge2_v2_y_index = edge2_indices_.second + this->image_vertices_count_;
	}

	virtual ~EdgePairAngleObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void CalculateValueInner(double& f) override
	{
		f = atan2(edge1_.y(), edge1_.x()) - atan2(edge2_.y(), edge2_.x());
	}
	
	void CalculateGradientInner(GradientType_& g) override
	{
		edge1_atan2_dx_ = -edge1_.y() / edge1_squared_norm_;
		edge1_atan2_dy_ = edge1_.x() / edge1_squared_norm_;
		edge2_atan2_dx_ = -edge2_.y() / edge2_squared_norm_;
		edge2_atan2_dy_ = edge2_.x() / edge2_squared_norm_;

		g.coeffRef(edge1_v1_x_index_) = edge1_atan2_dx_;
		g.coeffRef(edge1_v1_y_index_) = edge1_atan2_dy_;
		g.coeffRef(edge1_v2_x_index_) = -edge1_atan2_dx_;
		g.coeffRef(edge1_v2_y_index_) = -edge1_atan2_dy_;
		
		g.coeffRef(edge2_v1_x_index_) = edge2_atan2_dx_;
		g.coeffRef(edge2_v1_y_index_) = edge2_atan2_dy_;
		g.coeffRef(edge2_v2_x_index_) = -edge2_atan2_dx_;
		g.coeffRef(edge2_v2_y_index_) = -edge2_atan2_dy_;
	}
	
	void CalculateTripletsInner(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		edge1_v1_ = Eigen::Vector2d(x(edge1_v1_x_index_), x(edge1_v1_y_index_));
		edge1_v2_ = Eigen::Vector2d(x(edge1_v2_x_index_), x(edge1_v2_y_index_));
		edge2_v1_ = Eigen::Vector2d(x(edge2_v1_x_index_), x(edge2_v1_y_index_));
		edge2_v2_ = Eigen::Vector2d(x(edge2_v2_x_index_), x(edge2_v2_y_index_));
		edge1_ = edge1_v2_ - edge1_v1_;
		edge2_ = edge2_v2_ - edge2_v1_;
		edge1_squared_norm_ = edge1_.squaredNorm();
		edge2_squared_norm_ = edge2_.squaredNorm();
	}

private:
	/**
	 * Private overrides
	 */
	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.resize(36);
		auto triplet_index = 0;
		for(auto& row : indices_)
		{
			for (auto& column : indices_)
			{
				if(column >= row)
				{
					triplets[triplet_index] = Eigen::Triplet<double>(row, column, 0);
					triplet_index++;
				}
			}
		}
	}
	
	/**
	 * Private fields
	 */
	std::pair<uint64_t, uint64_t> edge1_indices_;
	std::pair<uint64_t, uint64_t> edge2_indices_;

	uint64_t edge1_v1_x_index_;
	uint64_t edge1_v1_y_index_;
	uint64_t edge1_v2_x_index_;
	uint64_t edge1_v2_y_index_;

	uint64_t edge2_v1_x_index_;
	uint64_t edge2_v1_y_index_;
	uint64_t edge2_v2_x_index_;
	uint64_t edge2_v2_y_index_;

	Eigen::Vector2d edge1_v1_;
	Eigen::Vector2d edge1_v2_;
	Eigen::Vector2d edge2_v1_;
	Eigen::Vector2d edge2_v2_;

	Eigen::Vector2d edge1_;
	Eigen::Vector2d edge2_;

	double edge1_atan2_dx_;
	double edge1_atan2_dy_;
	double edge2_atan2_dx_;
	double edge2_atan2_dy_;

	double edge1_squared_norm_;
	double edge2_squared_norm_;
};

#endif