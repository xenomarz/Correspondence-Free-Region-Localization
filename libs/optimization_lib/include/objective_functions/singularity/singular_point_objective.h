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
#include "../../data_providers/adjacent_faces_data_provider.h"

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
	SingularPointObjective(const std::string& name, const std::shared_ptr<AdjacentFacesDataProvider>& adjacent_faces_data_provider, double interval) :
		SummationObjective(adjacent_faces_data_provider, name, true),
		interval_(interval)
	{
		this->Initialize();
	}

	SingularPointObjective(double interval) :
		SingularPointObjective("Singular Point", interval)
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

	[[nodiscard]] std::shared_ptr<AdjacentFacesDataProvider> GetAdjacentFacesDataProvider() const
	{
		return std::static_pointer_cast<AdjacentFacesDataProvider>(this->data_provider_);
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
	 * Public methods
	 */
	void PreInitialize() override
	{
		const auto adjacent_faces_data_provider = GetAdjacentFacesDataProvider();
		auto adjacent_faces_vertices = adjacent_faces_data_provider->GetAdjacentFacesVertices();
		for (auto& adjacent_face_vertices : adjacent_faces_vertices)
		{
			auto x_coordinate_data_provider = std::make_shared<CoordinateDataProvider>(this->data_provider_->GetMeshDataProvider(), adjacent_face_vertices.first, CoordinateDataProvider::CoordinateType::X);
			auto y_coordinate_data_provider = std::make_shared<CoordinateDataProvider>(this->data_provider_->GetMeshDataProvider(), adjacent_face_vertices.first, CoordinateDataProvider::CoordinateType::Y);

			auto x_coordinate_objective = std::make_shared<CoordinateObjective<StorageOrder_>>(x_coordinate_data_provider);
			auto y_coordinate_objective = std::make_shared<CoordinateObjective<StorageOrder_>>(y_coordinate_data_provider);

			std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_x_coordinate_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(true, periodic_x_coordinate_objective, interval_);
			std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_y_coordinate_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(true, periodic_y_coordinate_objective, interval_);
			
			this->AddObjectiveFunction(periodic_x_coordinate_objective);
			this->AddObjectiveFunction(periodic_y_coordinate_objective);
		}
	}

private:

	/**
	 * Private overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		double weight = abs(GetAdjacentFacesDataProvider()->GetAngle() - 2 * M_PI);
		auto objective_functions_count = this->GetObjectiveFunctionsCount();
		for(std::size_t i = 0; i < objective_functions_count; i++)
		{
			this->GetObjectiveFunction(i)->SetWeight(weight);
		}
	}

	/**
	 * Private fields
	 */
	double interval_;
};

#endif