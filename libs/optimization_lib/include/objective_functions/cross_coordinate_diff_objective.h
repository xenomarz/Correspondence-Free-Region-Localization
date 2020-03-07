#pragma once
#ifndef OPTIMIZATION_LIB_CROSS_COORDINATE_DIFF_OBJECTIVE_H
#define OPTIMIZATION_LIB_CROSS_COORDINATE_DIFF_OBJECTIVE_H

// Optimization lib includes
#include "../core/core.h"
#include "../data_providers/cross_coordinate_diff_data_provider.h"
#include "./sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class CrossCoordinateDiffObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	CrossCoordinateDiffObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<CrossCoordinateDiffDataProvider>& cross_coordinate_diff_data_provider) :
		SparseObjectiveFunction(mesh_data_provider, cross_coordinate_diff_data_provider, "Cross Coordinate Diff Objective", 4, 4, false),
		cross_coordinate_diff_data_provider_(cross_coordinate_diff_data_provider)
	{
		this->Initialize();
	}

	virtual ~CrossCoordinateDiffObjective()
	{

	}

protected:
	void CalculateValue(double& f) override
	{
		f = cross_coordinate_diff_data_provider_->GetCrossCoordinateDiffValueSquared();
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		// 	coordinate_diff_value_ = x.coeff(edge1_variable1_index_) - x.coeff(edge2_variable1_index_) - (x.coeff(edge1_variable2_index_) - x.coeff(edge2_variable2_index_));

		const double coordinate_diff_value_doubled = 2 * cross_coordinate_diff_data_provider_->GetCrossCoordinateDiffValue();
		g.coeffRef(cross_coordinate_diff_data_provider_->GetEdge1Variable1Index()) = coordinate_diff_value_doubled;
		g.coeffRef(cross_coordinate_diff_data_provider_->GetEdge2Variable1Index()) = -coordinate_diff_value_doubled;
		g.coeffRef(cross_coordinate_diff_data_provider_->GetEdge1Variable2Index()) = -coordinate_diff_value_doubled;
		g.coeffRef(cross_coordinate_diff_data_provider_->GetEdge2Variable2Index()) = coordinate_diff_value_doubled;
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
		sparse_variable_indices.push_back(cross_coordinate_diff_data_provider_->GetEdge1Variable1Index());
		sparse_variable_indices.push_back(cross_coordinate_diff_data_provider_->GetEdge2Variable1Index());
		sparse_variable_indices.push_back(cross_coordinate_diff_data_provider_->GetEdge1Variable2Index());
		sparse_variable_indices.push_back(cross_coordinate_diff_data_provider_->GetEdge2Variable2Index());
	}

	/**
	 * Private fields
	 */
	std::shared_ptr<CrossCoordinateDiffDataProvider> cross_coordinate_diff_data_provider_;
};

#endif