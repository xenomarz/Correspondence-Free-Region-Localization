#pragma once
#ifndef OPTIMIZATION_LIB_BARYCENTER_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_BARYCENTER_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./position_objective.h"

template<Eigen::StorageOptions StorageOrder>
class BarycenterPositionObjective : public PositionObjective<StorageOrder>
{
public:
	/**
	 * Constructors and destructor
	 */
	BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<int64_t>& indices, const Eigen::Vector2d& objective_barycenter) :
		PositionObjective(objective_function_data_provider, 2.0 / static_cast<double>(indices.size()), indices.size(), "Barycenter Position Objective"),
		indices_(indices),
		objective_barycenter_(objective_barycenter)
	{
		this->Initialize();
	}

	BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const Eigen::VectorXi& indices, const Eigen::Vector2d& objective_barycenter) :
		BarycenterPositionObjective(objective_function_data_provider, std::vector<int64_t>(indices.data(), indices.data() + indices.rows()), objective_barycenter)
	{

	}

	~BarycenterPositionObjective()
	{

	}

	/**
	 * Public overrides
	 */
	void OffsetPositionConstraint(const Eigen::Vector2d& offset)
	{
		objective_barycenter_ += offset;
	}

private:
	/**
	 * Private overrides
	 */
	void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) override
	{
		f = barycenters_diff_.squaredNorm();
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		g.setZero();
		for (uint64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			const auto current_index = indices_[i];
			g(current_index) = (2.0f / 3.0f) * barycenters_diff_(0,0);
			g(current_index + this->image_vertices_count_) = (2.0f / 3.0f) * barycenters_diff_(1, 0);
		}
	}

	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.resize(this->objective_variables_count_);

		int64_t vertex_index;
		int64_t vertex_index_shifted;
		int64_t i_shifted;

		for (uint64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			vertex_index = indices_[i];
			vertex_index_shifted = vertex_index + this->image_vertices_count_;
			i_shifted = i + this->objective_vertices_count_;

			triplets[i] = Eigen::Triplet<double>(vertex_index, vertex_index, 0);
			triplets[i_shifted] = Eigen::Triplet<double>(vertex_index_shifted, vertex_index_shifted, 0);
		}
	}
	
	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		int64_t vertex_index;
		for (uint64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			const_cast<double&>(triplets[i].value()) = 2.0f / 9.0f;
			const_cast<double&>(triplets[i + this->objective_vertices_count_].value()) = 2.0f / 9.0f;
		}		
	}

	void PreUpdate(const Eigen::VectorXd& x)
	{
		auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
		Utils::CalculateBarycenter(indices_, X, current_barycenter_);
		barycenters_diff_ = current_barycenter_ - objective_barycenter_;
	}

	/**
	 * Fields
	 */
	std::vector<int64_t> indices_;
	Eigen::Vector2d objective_barycenter_;
	Eigen::Vector2d current_barycenter_;
	Eigen::Vector2d barycenters_diff_;
};

#endif