#pragma once
#ifndef OPTIMIZATION_LIB_FACE_BARYCENTER_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_FACE_BARYCENTER_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../core/core.h"
#include "../../data_providers/face_data_provider.h"
#include "./face_position_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class FaceBarycenterPositionObjective : public FacePositionObjective<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	FaceBarycenterPositionObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<FaceDataProvider>& face_data_provider, const Eigen::Vector2d& objective_barycenter) :
		FacePositionObjective(mesh_data_provider, face_data_provider, "Barycenter Position Objective"),
		objective_barycenter_(objective_barycenter)
	{
		const auto indices_count = face_data_provider->GetFace().size();
		gradient_coeff_ = 2.0 / indices_count;
		hessian_coeff_ = gradient_coeff_ / indices_count;
		this->Initialize();
	}

	~FaceBarycenterPositionObjective()
	{

	}

	/**
	 * Public overrides
	 */
	void MoveFacePosition(const Eigen::Vector2d& offset)
	{
		objective_barycenter_ += offset;
	}

private:
	/**
	 * Private overrides
	 */
	void CalculateValue(double& f) override
	{
		f = barycenters_diff_.squaredNorm();
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		g.setZero();
		auto face_data_provider = this->GetFaceDataProvider();
		for (int64_t i = 0; i < this->objective_variables_count_; i++)
		{
			const auto row = i % this->mesh_data_provider_->GetImageVerticesCount();
			auto face = face_data_provider->GetFace();
			const auto current_index = face[i];
			g(current_index) = gradient_coeff_ * barycenters_diff_(row, 0);
		}
	}

	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.resize(this->objective_variables_count_);
		auto face_data_provider = this->GetFaceDataProvider();
		auto face = face_data_provider->GetFace();
		for (int64_t i = 0; i < this->objective_variables_count_; i++)
		{
			int64_t offset = (i / this->objective_vertices_count_) * this->objective_vertices_count_;
			auto vertex_index = face[i % this->objective_vertices_count_];
			auto variable_index = vertex_index + offset;
			triplets[i] = Eigen::Triplet<double>(variable_index, variable_index, 0);
		}
	}
	
	void CalculateRawTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		for (int64_t i = 0; i < this->objective_variables_count_; i++)
		{
			const_cast<double&>(triplets[i].value()) = hessian_coeff_;
		}	
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		current_barycenter_ = this->GetFaceDataProvider()->GetBarycenter();
		barycenters_diff_ = current_barycenter_ - objective_barycenter_;
	}

	/**
	 * Fields
	 */
	double gradient_coeff_;
	double hessian_coeff_;
	Eigen::Vector2d objective_barycenter_;
	Eigen::Vector2d current_barycenter_;
	Eigen::Vector2d barycenters_diff_;
};

#endif