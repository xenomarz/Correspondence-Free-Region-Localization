#pragma once
#ifndef OPTIMIZATION_LIB_PERIODIC_OBJECTIVE_H
#define OPTIMIZATION_LIB_PERIODIC_OBJECTIVE_H

// STL includes
#include <vector>
#include <cmath>

// Optimization lib includes
#include "./concrete_objective.h"

template<Eigen::StorageOptions StorageOrder>
class PeriodicObjective : public ConcreteObjective<StorageOrder>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : uint32_t
	{
		Period = ObjectiveFunction<StorageOrder>::Properties::Count_
	};
	
	/**
	 * Constructors and destructor
	 */
	PeriodicObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double period) :
		ConcreteObjective<StorageOrder>(objective_function_data_provider, "Periodic")
	{
		SetPeriod(period);
		this->Initialize();
	}

	virtual ~PeriodicObjective()
	{

	}

	/**
	 * Setters
	 */
	void SetPeriod(const double period)
	{
		period_ = period;
		period_squared_ = period * period;
		period_tripled_ = period_squared_ * period;
		
		Eigen::Vector4d b;
		b << 0, 0, 0, 1;

		Eigen::Matrix4d A;
		A << 0,					  0,				 1,		  0,
			 3 * period_squared_, 2 * period_,		 1,		  0,
			 0,					  0,				 0,		  1,
			 period_tripled_,	  period_squared_,	 period_, 1;

		polynomial_coeffs_ = A.fullPivHouseholderQr().solve(b);
	}

	bool SetProperty(const uint32_t property_id, const std::any& property_value) override
	{
		if (ObjectiveFunction<StorageOrder>::SetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Period:
			SetPeriod(std::any_cast<const double>(property_value));
			return true;
		}

		return false;
	}

	/**
	 * Getters
	 */
	double GetPeriod() const
	{
		return period_;
	}

	double GetPeriodSquared() const
	{
		return period_squared_;
	}

	double GetPeriodTripled() const
	{
		return period_tripled_;
	}

	const Eigen::Vector4d& GetPolynomialCoeffs() const
	{
		return polynomial_coeffs_;
	}

	bool GetProperty(const uint32_t property_id, std::any& property_value) override
	{
		if (ObjectiveFunction<StorageOrder>::GetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Period:
			property_value = GetPeriod();
			return true;
		}

		return false;
	}

protected:
	double CalculatePolynomial(const double x)
	{
		
	}
	
	double CalculatePolynomialFirstDerivative(const double x)
	{
		
	}

	double CalculatePolynomialSecondDerivative(const double x)
	{

	}

private:
	/**
	 * Fields
	 */
	double period_;
	double period_squared_;
	double period_tripled_;
	Eigen::Vector4d polynomial_coeffs_;
};

#endif