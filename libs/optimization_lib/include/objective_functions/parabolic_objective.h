#pragma once
#ifndef OPTIMIZATION_LIB_PARABOLIC_OBJECTIVE_H
#define OPTIMIZATION_LIB_PARABOLIC_OBJECTIVE_H

// STL includes
#include <vector>
#include <cmath>

// Optimization lib includes
#include "../data_providers/plain_data_provider.h"
#include "./composite_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class ParabolicObjective : public CompositeObjective<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Root = SparseObjectiveFunction<StorageOrder_>::Properties::Count_
	};

	/**
	 * Constructors and destructor
	 */
	ParabolicObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::string& name, const std::shared_ptr<SparseObjectiveFunction<StorageOrder_>>& inner_objective, const double c0, const double c1, const double c2, const bool enforce_psd = true) :
		CompositeObjective(mesh_data_provider, empty_data_provider, name, enforce_psd, inner_objective)
	{
		polynomial_coeffs_.resize(3);
		SetC0(c0);
		SetC1(c1);
		SetC2(c2);
		this->Initialize();
	}

	ParabolicObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::shared_ptr<SparseObjectiveFunction<StorageOrder_>>& inner_objective, const double c0, const double c1, const double c2, const bool enforce_psd = true) :
		ParabolicObjective(mesh_data_provider, empty_data_provider, "Parabolic Objective", inner_objective, c0, c1, c2, enforce_psd)
	{

	}

	virtual ~ParabolicObjective() override
	{

	}

	/**
	 * Setters
	 */
	void SetC0(const double c0)
	{
		polynomial_coeffs_(0) = c0;
	}

	void SetC1(const double c1)
	{
		polynomial_coeffs_(1) = c1;
	}

	void SetC2(const double c2)
	{
		polynomial_coeffs_(2) = c2;
	}

	/**
	 * Getters
	 */
	double GetC0() const
	{
		return polynomial_coeffs_(0);
	}

	double GetC1() const
	{
		return polynomial_coeffs_(1);
	}

	double GetC2() const
	{
		return polynomial_coeffs_(2);
	}

private:
	/**
	 * Private overrides
	 */
	void CalculateDerivativesOuter(const double x, double& outer_value, double& outer_first_derivative, double& outer_second_derivative) override
	{
		const double f = x;
		const double f2 = f * f;

		Eigen::VectorXd values(3);

		values << 1, f, f2;
		outer_value = values.dot(polynomial_coeffs_);

		values << 0, 1, 2 * f;
		outer_first_derivative = values.dot(polynomial_coeffs_);

		values << 0, 0, 2;
		outer_second_derivative = values.dot(polynomial_coeffs_);
	}

	/**
	 * Private fields
	 */
	Eigen::VectorXd polynomial_coeffs_;
};

#endif