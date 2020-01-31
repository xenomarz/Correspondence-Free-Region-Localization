#pragma once
#ifndef OPTIMIZATION_LIB_COORDINATE_DIFF_OBJECTIVE_H
#define OPTIMIZATION_LIB_COORDINATE_DIFF_OBJECTIVE_H

// Optimization lib includes
#include "../core/core.h"
#include "../data_providers/coordinate_diff_data_provider.h"
#include "./sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class CoordinateDiffObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	CoordinateDiffObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<CoordinateDiffDataProvider>& coordinate_diff_data_provider) :
		SparseObjectiveFunction(mesh_data_provider, coordinate_diff_data_provider, "Coordinate Diff Objective", 2, 2, false),
		coordinate_diff_data_provider_(coordinate_diff_data_provider)
	{
		this->Initialize();
	}

	virtual ~CoordinateDiffObjective()
	{

	}

protected:
	void CalculateValue(double& f) override
	{
		f = coordinate_diff_data_provider_->GetCoordinateDiffValue();
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		g.coeffRef(coordinate_diff_data_provider_->GetSparseVariable1Index()) = 1;
		g.coeffRef(coordinate_diff_data_provider_->GetSparseVariable2Index()) = -1;
	}

	void CalculateRawTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		// Empty implementation
	}

private:
	/**
	 * Private overrides
	 */
	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		sparse_variable_indices.push_back(coordinate_diff_data_provider_->GetSparseVariable1Index());
		sparse_variable_indices.push_back(coordinate_diff_data_provider_->GetSparseVariable2Index());
	}

	/**
	 * Private fields
	 */
	std::shared_ptr<CoordinateDiffDataProvider> coordinate_diff_data_provider_;
};

#endif