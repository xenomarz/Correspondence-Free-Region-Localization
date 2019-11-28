#pragma once
#ifndef OPTIMIZATION_LIB_SINGULAR_POINT_OBJECTIVE_H
#define OPTIMIZATION_LIB_SINGULAR_POINT_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../summation_objective.h"
#include "../coordinate_objective.h"
#include "../periodic_objective.h"
#include "../../data_providers/face_fan_data_provider.h"

template <Eigen::StorageOptions StorageOrder_>
class SingularPointObjective : public SummationObjective<PeriodicObjective<StorageOrder_>>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Interval = SummationObjective<PeriodicObjective<StorageOrder_>>::Properties::Count_
	};

	/**
	 * Constructors and destructor
	 */
	SingularPointObjective(const std::shared_ptr<FaceFanDataProvider>& face_fan_data_provider, const std::string& name, const double interval) :
		SummationObjective(face_fan_data_provider, name, false, false),
		interval_(interval)
	{
		this->Initialize();
	}

	SingularPointObjective(const std::shared_ptr<FaceFanDataProvider>& face_fan_data_provider, const double interval) :
		SingularPointObjective(face_fan_data_provider, "Singular Point", interval)
	{

	}

	virtual ~SingularPointObjective()
	{

	}

	/**
	 * Setters
	 */
	void SetInterval(const double interval)
	{
		for (int64_t i = 0; i < this->GetObjectiveFunctionsCount(); i++)
		{
			this->GetObjectiveFunction(i)->SetPeriod(interval);
		}
		interval_ = interval;
	}

	bool SetProperty(const int32_t property_id, const std::any& property_value) override
	{
		if (SummationObjective<PeriodicObjective<StorageOrder_>>::SetProperty(property_id, property_value))
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

	[[nodiscard]] std::shared_ptr<FaceFanDataProvider> GetFaceFanDataProvider() const
	{
		return std::static_pointer_cast<FaceFanDataProvider>(this->data_provider_);
	}

	bool GetProperty(const int32_t property_id, std::any& property_value) override
	{
		if (SummationObjective<PeriodicObjective<StorageOrder_>>::GetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Interval:
			property_value = GetInterval();
			return true;
		}

		return false;
	}

	/**
	 * Public overrides
	 */
	void PreInitialize() override
	{
		const auto face_fan_data_provider = GetFaceFanDataProvider();
		auto face_fan = face_fan_data_provider->GetFaceFan();
		for (auto& face_fan_slice : face_fan)
		{
			auto x_coordinate_data_provider = std::make_shared<CoordinateDataProvider>(this->data_provider_->GetMeshDataProvider(), face_fan_slice.first, CoordinateDataProvider::CoordinateType::X);
			auto y_coordinate_data_provider = std::make_shared<CoordinateDataProvider>(this->data_provider_->GetMeshDataProvider(), face_fan_slice.first, CoordinateDataProvider::CoordinateType::Y);

			auto x_coordinate_objective = std::make_shared<CoordinateObjective<StorageOrder_>>(x_coordinate_data_provider);
			auto y_coordinate_objective = std::make_shared<CoordinateObjective<StorageOrder_>>(y_coordinate_data_provider);

			std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_x_coordinate_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(x_coordinate_objective, interval_, true);
			std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_y_coordinate_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(y_coordinate_objective, interval_, true);
			
			this->AddObjectiveFunction(periodic_x_coordinate_objective);
			this->AddObjectiveFunction(periodic_y_coordinate_objective);
		}
	}

	/**
	 * Public methods
	 */
	[[nodiscard]] double GetSingularityWeight() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return singular_weight_;
	}

	[[nodiscard]] std::vector<RDS::VertexIndex> GetSingularVertexIndices() const
	{
		std::lock_guard<std::mutex> lock(m_);
		std::vector<RDS::VertexIndex> singular_vertex_indices;
		const auto face_fan_data_provider = GetFaceFanDataProvider();
		auto face_fan = face_fan_data_provider->GetFaceFan();
		const auto face_fan_size = face_fan.size();
		for (std::size_t face_fan_index = 0; face_fan_index < face_fan_size; face_fan_index++)
		{
			singular_vertex_indices.push_back(face_fan[face_fan_index].first);
		}

		return singular_vertex_indices;
	}
	
private:

	/**
	 * Private overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x, UpdatableObject::UpdatedObjectSet& updated_objects) override
	{
		singular_weight_ = abs(GetFaceFanDataProvider()->GetAngle() - 2 * M_PI);
		auto objective_functions_count = this->GetObjectiveFunctionsCountInternal();
		for(std::size_t i = 0; i < objective_functions_count; i++)
		{
			this->GetObjectiveFunctionInternal(i)->SetWeight(singular_weight_);
		}

		SummationObjective<PeriodicObjective<StorageOrder_>>::PreUpdate(x, updated_objects);
	}

	/**
	 * Private fields
	 */
	double interval_;
	double singular_weight_;
};

#endif