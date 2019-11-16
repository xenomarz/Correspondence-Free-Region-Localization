#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>

// Optimization lib includes
#include "../core/core.h"
#include "./sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class CompositeObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::string& name, const bool enforce_psd, const std::shared_ptr<SparseObjectiveFunction<StorageOrder_>>& inner_objective) :
		SparseObjectiveFunction(inner_objective->GetDataProvider(), name, inner_objective->GetObjectiveVerticesCount(), inner_objective->GetObjectiveVariablesCount(), enforce_psd),
		inner_objective_(inner_objective)
	{

	}

	CompositeObjective(const bool enforce_psd, const std::shared_ptr<SparseObjectiveFunction<StorageOrder_>>& inner_objective) :
		CompositeObjective(inner_objective->GetDataProvider(), "Composite Objective", enforce_psd, inner_objective)
	{

	}

	virtual ~CompositeObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		inner_objective_->Update(x);
		CalculateDerivativesOuter(inner_objective_->GetValue(), outer_value_, outer_first_derivative_, outer_second_derivative_);
	}

	/**
	 * Value, first derivative and second derivative calculation for outer function (f: R -> R)
	 */
	virtual void CalculateDerivativesOuter(const double x, double& outer_value, double& outer_first_derivative, double& outer_second_derivative) = 0;

private:
	/**
	 * Private method overrides
	 */
	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		sparse_variable_indices = inner_objective_->GetSparseVariablesIndices();
	}

	void CalculateValue(double& f) override
	{
		f = outer_value_;
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		g = outer_first_derivative_ * inner_objective_->GetGradient();
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const auto triplets_count = triplets.size();
		auto& g_inner = inner_objective_->GetGradient();
		auto& triplets_inner = inner_objective_->GetTriplets();
		for (std::size_t i = 0; i < triplets_count; i++)
		{
			const_cast<double&>(triplets[i].value()) = (outer_first_derivative_ * triplets_inner[i].value()) + (outer_second_derivative_ * g_inner.coeff(triplets_inner[i].row()) * g_inner.coeff(triplets_inner[i].col()));
		}
	}

	/**
	 * Private fields
	 */
	double outer_value_;
	double outer_first_derivative_;
	double outer_second_derivative_;
	std::shared_ptr<SparseObjectiveFunction<StorageOrder_>> inner_objective_;
};

#endif