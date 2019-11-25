#pragma once
#ifndef OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../core/core.h"
#include "../../data_providers/plain_data_provider.h"
#include "./position_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class VertexPositionObjective : public PositionObjective<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	VertexPositionObjective(const std::shared_ptr<PlainDataProvider>& plain_data_provider, const std::vector<std::pair<RDS::VertexIndex, Eigen::Vector2d>>& index_vertex_pairs) :
		PositionObjective(plain_data_provider, "Vertex Position Objective", index_vertex_pairs.size()),
		index_vertex_pairs_(index_vertex_pairs)
	{
		X_current_.resize(this->objective_vertices_count_, 2);
		X_objective_.resize(this->objective_vertices_count_, 2);
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			X_objective_.row(i) = index_vertex_pairs[i].second;
		}
		this->Initialize();
	}

	virtual ~VertexPositionObjective()
	{

	}

	void OffsetPositionConstraint(const Eigen::Vector2d& offset) override
	{
		X_objective_ = X_objective_.rowwise() + offset.transpose();
	}

private:
	/**
	 * Overrides
	 */

	void CalculateValue(double& f) override
	{
		f = X_diff_.squaredNorm();
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		auto image_vertices_count = this->data_provider_->GetMeshDataProvider()->GetImageVerticesCount();
		g.setZero();
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			const auto current_index = index_vertex_pairs_[i].first;
			g(current_index) = 2.0 * X_diff_(i, 0);
			g(current_index + image_vertices_count) = 2.0 * X_diff_(i, 1);
		}
	}

	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		auto image_vertices_count = this->data_provider_->GetMeshDataProvider()->GetImageVerticesCount();
		triplets.resize(this->objective_variables_count_);
		int64_t current_index;
		int64_t current_index_shifted;
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			current_index = index_vertex_pairs_[i].first;
			current_index_shifted = index_vertex_pairs_[i].first + image_vertices_count;
			triplets[i] = Eigen::Triplet<double>(current_index, current_index, 0);
			triplets[i + this->objective_vertices_count_] = Eigen::Triplet<double>(current_index_shifted, current_index_shifted, 0);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		for (int64_t i = 0; i < this->objective_variables_count_; i++)
		{
			const_cast<double&>(triplets[i].value()) = 2.0;
		}
	}

	void PreUpdate(const Eigen::VectorXd& x, UpdatableObject::UpdatedObjectSet& updated_objects) override
	{
		auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			X_current_.row(i) = X.row(this->index_vertex_pairs_[i].first);
		}
		X_diff_ = X_current_ - X_objective_;
	}

	/**
	 * Fields
	 */
	std::vector<std::pair<RDS::VertexIndex, Eigen::Vector2d>> index_vertex_pairs_;
	Eigen::MatrixX2d X_objective_;
	Eigen::MatrixX2d X_current_;
	Eigen::MatrixX2d X_diff_;
};

#endif