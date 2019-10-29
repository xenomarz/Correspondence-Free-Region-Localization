#pragma once
#ifndef OPTIMIZATION_LIB_PERIODIC_OBJECTIVE_H
#define OPTIMIZATION_LIB_PERIODIC_OBJECTIVE_H

// STL includes
#include <vector>
#include <cmath>

// Optimization lib includes
#include "./sparse_objective_function.h"
#include "./concrete_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class PeriodicObjective : public ConcreteObjective<SparseObjectiveFunction<StorageOrder_>>
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
	PeriodicObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name, const double period) :
		ConcreteObjective(objective_function_data_provider, name)
	{
		SetPeriod(period);
	}
	
	PeriodicObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double period) :
		PeriodicObjective(objective_function_data_provider, "Periodic", period)
	{

	}

	virtual ~PeriodicObjective()
	{

	}

	/**
	 * Setters
	 */
	void SetPeriod(const double period)
	{
		p_ = period;
		p2_ = p_ * p_;
		p3_ = p2_ * p_;
		p4_ = p3_ * p_;
		p5_ = p4_ * p_;

		hp_ = period / 2;
		hp2_ = hp_ * hp_;
		hp3_ = hp2_ * hp_;
		hp4_ = hp3_ * hp_;
		hp5_ = hp4_ * hp_;

		Eigen::VectorXd b(6);
		b << 0, 0, 1, 0, 0, 0;

		Eigen::MatrixXd A(6,6);
		A <<		0,		   0,		   0,			0 ,		  0,	 1,
					0,		   0,		   0,			0 ,		  1,	 0,
				 hp5_,		hp4_,		hp3_,		  hp2_,		hp_,	 1,
			 5 * hp4_,	4 * hp3_,	3 * hp2_,	  2 * hp_ ,		  1,	 0,
				  p5_,		 p4_,		 p3_,		   p2_,		 p_,	 1,
			 5 *  p4_,	4 *  p3_,	3 *  p2_,	  2 *  p_ ,		  1,	 0;

		polynomial_coeffs_ = A.fullPivHouseholderQr().solve(b);
		polynomial_coeffs_.coeffRef(0) = 0;
		polynomial_coeffs_.coeffRef(4) = 0;
		polynomial_coeffs_.coeffRef(5) = 0;
	}

	bool SetProperty(const uint32_t property_id, const std::any& property_value) override
	{
		if (SparseObjectiveFunction<StorageOrder_>::SetProperty(property_id, property_value))
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
		return p_;
	}

	double GetPeriodSquared() const
	{
		return p2_;
	}

	double GetPeriodTripled() const
	{
		return p3_;
	}

	const Eigen::VectorXd& GetPolynomialCoeffs() const
	{
		return polynomial_coeffs_;
	}

	bool GetProperty(const uint32_t property_id, std::any& property_value) override
	{
		if (SparseObjectiveFunction<StorageOrder_>::GetProperty(property_id, property_value))
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
	/**
	 * Value, gradient and hessian calculation functions for inner function
	 */
	virtual void CalculateValueInner(double& f) = 0;
	virtual void CalculateGradientInner(Eigen::SparseVector<double>& g) = 0;
	virtual void CalculateTripletsInner(std::vector<Eigen::Triplet<double>>& triplets) = 0;

	/**
	 * Value, gradient and hessian calculation functions for outer function
	 */
	void CalculateValueOuter(double& f)
	{
		f = polynomial_value_;
	}
	
	void CalculateGradientOuter(Eigen::SparseVector<double>& g)
	{
		g = polynomial_first_derivative_ * g;
	}
	
	void CalculateTripletsOuter(std::vector<Eigen::Triplet<double>>& triplets)
	{
		auto g = this->GetGradientInternal();
		const auto triplets_count = triplets.size();
		for(std::size_t i = 0; i < triplets_count; i++)
		{
			auto& value = const_cast<double&>(triplets[i].value());
			value = (polynomial_first_derivative_ * value) + (polynomial_second_derivative_ * g.coeffRef(triplets[i].row()) * g.coeffRef(triplets[i].col()));
		}
	}

private:
	/**
	 * Private method overrides
	 */
	void CalculateValue(double& f) override
	{
		CalculateValueInner(f);
		CalculatePolynomialDerivatives(fmod(f, p_));
		CalculateValueOuter(f);
	}
	
	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		CalculateGradientInner(g);
		CalculateGradientOuter(g);
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		CalculateTripletsInner(triplets);
		CalculateTripletsOuter(triplets);
	}
	
	/**
	 * Private methods
	 */
	void CalculatePolynomialDerivatives(const double f)
	{
		const double f2 = f * f;
		const double f3 = f2 * f;
		const double f4 = f3 * f;
		const double f5 = f4 * f;
		
		Eigen::VectorXd values(6);

		values << f5, f4, f3, f2, f, 1;
		polynomial_value_ = values.dot(polynomial_coeffs_);

		values << 5 * f4, 4 * f3, 3 * f2, 2 * f, 1, 0;
		polynomial_first_derivative_ = values.dot(polynomial_coeffs_);

		values << 20 * f3, 12 * f2, 6 * f, 2, 0, 0;
		polynomial_second_derivative_ = values.dot(polynomial_coeffs_);
	}
	
	/**
	 * Private fields
	 */
	double polynomial_value_;
	double polynomial_first_derivative_;
	double polynomial_second_derivative_;
	double hp_;
	double hp2_;
	double hp3_;
	double hp4_;
	double hp5_;
	double p_;
	double p2_;
	double p3_;
	double p4_;
	double p5_;
	Eigen::VectorXd polynomial_coeffs_;
};

#endif