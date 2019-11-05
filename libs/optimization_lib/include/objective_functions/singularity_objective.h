#pragma once
#ifndef OPTIMIZATION_LIB_SINGULARITY_OBJECTIVE_H
#define OPTIMIZATION_LIB_SINGULARITY_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./composite_objective.h"
#include "./integer_objective.h"
#include <corecrt_math_defines.h>

template <Eigen::StorageOptions StorageOrder_>
class SingularityObjective : public CompositeObjective<IntegerObjective<StorageOrder_>>
{
public:
	/**
	 * Public type definitions
	 */
	using SingularCorner = std::pair<int64_t, std::vector<int64_t>>;
	
	/**
	 * Constructors and destructor
	 */
	SingularityObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		CompositeObjective(objective_function_data_provider, name, false, true)
	{
		this->Initialize();
	}

	SingularityObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
		SingularityObjective(objective_function_data_provider, "Singularity")
	{

	}

	virtual ~SingularityObjective()
	{

	}

	/**
	 * Public methods
	 */
	void AddSingularCorners(const std::vector<SingularCorner>& singular_corners)
	{
		singular_corner_clusters_.push_back(singular_corners);

		for(auto& singular_corner : singular_corners)
		{
			auto x_component_objective = std::make_shared<IntegerObjective<StorageOrder_>>(this->objective_function_data_provider_, singular_corner.first);
			auto y_component_objective = std::make_shared<IntegerObjective<StorageOrder_>>(this->objective_function_data_provider_, singular_corner.first + this->image_vertices_count_);
			
			this->AddObjectiveFunction(x_component_objective);
			this->AddObjectiveFunction(y_component_objective);

			singular_corner_to_integer_objectives_[singular_corner.first].push_back(x_component_objective);
			singular_corner_to_integer_objectives_[singular_corner.first].push_back(y_component_objective);
		}
	}

	void AddSingularCornersTest()
	{
		auto x_component_objective = std::make_shared<IntegerObjective<StorageOrder_>>(this->objective_function_data_provider_, 0);
		auto y_component_objective = std::make_shared<IntegerObjective<StorageOrder_>>(this->objective_function_data_provider_, this->image_vertices_count_);
		
		this->AddObjectiveFunction(x_component_objective);
		this->AddObjectiveFunction(y_component_objective);
	}

private:

	/**
	 * Private overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		//const auto singular_corner_clusters_size = singular_corner_clusters_.size();
		//for(int64_t cluster_index = 0; cluster_index < singular_corner_clusters_size; cluster_index++)
		//{
		//	double accumulated_angle = 0;
		//	auto& singular_corners = singular_corner_clusters_[cluster_index];
		//	const auto singular_corner_cluster_size = singular_corners.size();
		//	for (int64_t singular_corner_index = 0; singular_corner_index < singular_corner_cluster_size; singular_corner_index++)
		//	{
		//		SingularCorner& singular_corner = singular_corners[singular_corner_index];
		//		int64_t v0_index = singular_corner.first;
		//		int64_t v1_index = singular_corner.second[0];
		//		int64_t v2_index = singular_corner.second[1];
		//		
		//		Eigen::Vector2d v0;
		//		Eigen::Vector2d v1;
		//		Eigen::Vector2d v2;

		//		v0.coeffRef(0) = x.coeffRef(v0_index);
		//		v0.coeffRef(1) = x.coeffRef(v0_index + this->image_vertices_count_);

		//		v1.coeffRef(0) = x.coeffRef(v1_index);
		//		v1.coeffRef(1) = x.coeffRef(v1_index + this->image_vertices_count_);

		//		v2.coeffRef(0) = x.coeffRef(v2_index);
		//		v2.coeffRef(1) = x.coeffRef(v2_index + this->image_vertices_count_);	

		//		Eigen::Vector2d e1 = v1 - v0;
		//		Eigen::Vector2d e2 = v2 - v0;

		//		e1.normalize();
		//		e2.normalize();

		//		const double current_angle = abs(acos(e1.dot(e2)));
		//		accumulated_angle += current_angle;
		//	}

		//	double weight = 1 - (accumulated_angle / (2 * M_PI));

		//	for (int64_t singular_corner_index = 0; singular_corner_index < singular_corner_cluster_size; singular_corner_index++)
		//	{
		//		SingularCorner& singular_corner = singular_corners[singular_corner_index];
		//		auto& integer_objectives = singular_corner_to_integer_objectives_[singular_corner.first];
		//		for(auto& integer_objective : integer_objectives)
		//		{
		//			integer_objective->SetWeight(weight);
		//		}
		//	}
		//}

		CompositeObjective<IntegerObjective<StorageOrder_>>::PreUpdate(x);
	}
	
	/**
	 * Private fields
	 */
	std::vector<std::vector<SingularCorner>> singular_corner_clusters_;
	std::unordered_map<int64_t, std::vector<std::shared_ptr<IntegerObjective<StorageOrder_>>>> singular_corner_to_integer_objectives_;
};

#endif