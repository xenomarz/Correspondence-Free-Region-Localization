#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>

// Optimization lib includes
#include "../utils/type_definitions.h"
#include "../utils/utils.h"
#include "./sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class CompositeObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const bool enforce_psd, const std::shared_ptr<SparseObjectiveFunction<StorageOrder_>>& inner_objective) :
		SparseObjectiveFunction(mesh_data_provider, data_provider, name, objective_vertices_count, enforce_psd),
		inner_objective_(inner_objective)
	{

	}

	CompositeObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const double period, bool enforce_psd) :
		CompositeObjective(mesh_data_provider, data_provider, "Composite Objective", period, enforce_psd)
	{

	}

	virtual ~CompositeObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreInitialize() override
	{
		inner_objective_->Initialize();
	}

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
		for (std::size_t i = 0; i < triplets_count; i++)
		{
			auto& value = const_cast<double&>(triplets[i].value());
			value = (outer_first_derivative_ * value) + (outer_second_derivative_ * g_inner.coeffRef(triplets[i].row()) * g_inner.coeffRef(triplets[i].col()));
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