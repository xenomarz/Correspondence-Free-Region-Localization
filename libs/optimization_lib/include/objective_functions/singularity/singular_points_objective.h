#pragma once
#ifndef OPTIMIZATION_LIB_SINGULAR_POINTS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SINGULAR_POINTS_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../../data_providers/empty_data_provider.h"
#include "../summation_objective.h"
#include "./singular_point_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SingularPointsObjective : public SummationObjective<SingularPointObjective<StorageOrder_>, Eigen::VectorXd>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Interval = SummationObjective<SingularPointObjective<StorageOrder_>, Eigen::VectorXd>::Properties::Count_,
		SingularityWeightPerVertex
	};

	
	/**
	 * Constructors and destructor
	 */
	SingularPointsObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::string& name, double interval, const bool enforce_children_psd = true) :
		SummationObjective(empty_data_provider, name, false, enforce_children_psd, false),
		interval_(interval)
	{
		this->Initialize();
	}

	SingularPointsObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, double interval, const bool enforce_children_psd = true) :
		SingularPointsObjective(empty_data_provider, "Singular Points", interval, enforce_children_psd)
	{

	}

	virtual ~SingularPointsObjective()
	{

	}

	/**
	 * Setters
	 */
	void SetInterval(const double interval)
	{
		for (int64_t i = 0; i < this->GetObjectiveFunctionsCount(); i++)
		{
			this->GetObjectiveFunction(i)->SetInterval(interval);
		}
		interval_ = interval;
	}

	bool SetProperty(const int32_t property_id, const std::any& property_value) override
	{
		if (SummationObjective<SingularPointObjective<StorageOrder_>, Eigen::VectorXd>::SetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Interval:
			SetInterval(std::any_cast<const double>(property_value));
			return true;
		}

		return false;
	}

	/**
	 * Getters
	 */
	double GetInterval() const
	{
		return interval_;
	}

	const Eigen::VectorXd& GetSingularityWeightPerVertex() const
	{
		return singularity_weight_per_vertex_;
	}

	bool GetProperty(const int32_t property_id, std::any& property_value) override
	{
		if (SummationObjective<SingularPointObjective<StorageOrder_>, Eigen::VectorXd>::GetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Interval:
			property_value = GetInterval();
			return true;
		case Properties::SingularityWeightPerVertex:
			property_value = GetSingularityWeightPerVertex();
			return true;
		}

		return false;
	}

	/**
	 * Public overrides
	 */
	void PreInitialize() override
	{
		SummationObjective<SingularPointObjective<StorageOrder_>, Eigen::VectorXd>::PreInitialize();
		singularity_weight_per_vertex_.resize(this->GetDataProvider()->GetMeshDataProvider()->GetImageVerticesCount());
	}

	/**
	 * Public methods
	 */
	void AddSingularPointObjective(const std::shared_ptr<FaceFanDataProvider>& face_fan_data_provider)
	{
		this->AddObjectiveFunction(std::make_shared<SingularPointObjective<StorageOrder_>>(face_fan_data_provider, interval_, this->GetEnforceChildrenPsd()));
	}

protected:
	/**
	 * Protected overrides
	 */
	void PostUpdate(const Eigen::VectorXd& x, UpdatableObject::UpdatedObjectSet& updated_objects) override
	{
		singularity_weight_per_vertex_.setZero();
		for (int64_t i = 0; i < this->GetObjectiveFunctionsCountInternal(); i++)
		{
			auto objective_function = this->GetObjectiveFunctionInternal(i);
			const std::vector<RDS::VertexIndex> singular_vertex_indices = objective_function->GetSingularVertexIndices();
			const double singularity_weight = objective_function->GetSingularityWeight();
			const auto singular_vertex_indices_size = singular_vertex_indices.size();
			for(std::size_t i = 0; i < singular_vertex_indices_size; i++)
			{
				singularity_weight_per_vertex_.coeffRef(singular_vertex_indices.at(i)) += singularity_weight;
			}
		}
	}

private:
	/**
	 * Private fields
	 */
	double interval_;
	Eigen::VectorXd singularity_weight_per_vertex_;
};

#endif