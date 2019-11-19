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
class SingularPointsObjective : public SummationObjective<SingularPointObjective<StorageOrder_>>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Interval = SummationObjective<SingularPointObjective<StorageOrder_>>::Properties::Count_
	};

	
	/**
	 * Constructors and destructor
	 */
	SingularPointsObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::string& name, double interval) :
		SummationObjective(empty_data_provider, name, false, false)
	{

	}

	SingularPointsObjective(const std::shared_ptr<EmptyDataProvider>& empty_data_provider, double interval) :
		SingularPointsObjective(empty_data_provider, "Singular Points", interval)
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
		if (SummationObjective<SingularPointObjective<StorageOrder_>>::SetProperty(property_id, property_value))
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

	bool GetProperty(const int32_t property_id, std::any& property_value) override
	{
		if (SummationObjective<SingularPointObjective<StorageOrder_>>::GetProperty(property_id, property_value))
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
	void AddSingularPointObjective(const std::shared_ptr<FaceFanDataProvider>& face_fan_data_provider)
	{
		this->AddObjectiveFunction(std::make_shared<SingularPointObjective<StorageOrder_>>(face_fan_data_provider, interval_));
	}

private:
	/**
	 * Private fields
	 */
	double interval_;
};

#endif