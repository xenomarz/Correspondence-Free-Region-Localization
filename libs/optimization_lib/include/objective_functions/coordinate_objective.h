#pragma once
#ifndef OPTIMIZATION_LIB_COORDINATE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COORDINATE_OBJECTIVE_H

// Optimization lib includes
#include "../core/core.h"
#include "../data_providers/coordinate_data_provider.h"
#include "./sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class CoordinateObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Period = SparseObjectiveFunction<StorageOrder_>::Properties::Count_
	};

	/**
	 * Constructors and destructor
	 */
	CoordinateObjective(const std::shared_ptr<CoordinateDataProvider>& coordinate_data_provider) :
		SparseObjectiveFunction(coordinate_data_provider, "Integer Objective", 1, 1, false),
		coordinate_data_provider_(coordinate_data_provider)
	{
		this->Initialize();
	}

	virtual ~CoordinateObjective()
	{

	}

protected:
	void CalculateValue(double& f) override
	{
		f = coordinate_data_provider_->GetCoordinateValue();
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		g.coeffRef(coordinate_data_provider_->GetSparseVariableIndex()) = 1;
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const auto sparse_variable_index = coordinate_data_provider_->GetSparseVariableIndex();
		triplets[0] = Eigen::Triplet<double>(sparse_variable_index, sparse_variable_index, 0);
	}

private:
	/**
	 * Private overrides
	 */
	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		sparse_variable_indices.push_back(coordinate_data_provider_->GetSparseVariableIndex());
	}

	/**
	 * Private fields
	 */
	std::shared_ptr<CoordinateDataProvider> coordinate_data_provider_;
};

#endif