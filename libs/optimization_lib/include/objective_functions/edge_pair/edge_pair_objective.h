#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_OBJECTIVE_H

// STL includes
#include <utility>
#include <unordered_map>

// Optimization lib includes
#include "../../core/core.h"
#include "../../data_providers/edge_pair_data_provider.h"
#include "../sparse_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class EdgePairObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairObjective(const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider, const std::string& name, const bool enforce_psd) :
		SparseObjectiveFunction(edge_pair_data_provider, name, 4, enforce_psd)
	{

	}

	virtual ~EdgePairObjective()
	{

	}

	const EdgePairDataProvider& GetEdgePairDataProvider() const
	{
		return *std::dynamic_pointer_cast<EdgePairDataProvider>(this->data_provider_);
	}

protected:
	/**
	 * Protected overrides
	 */
	void PostInitialize() override
	{
		SparseObjectiveFunction<StorageOrder_>::PostInitialize();

		auto& edge_pair_data_provider = this->GetEdgePairDataProvider();
		auto& dense_variable_index_to_sparse_variable_index_map = this->GetDenseVariableIndexToSparseVariableIndexMap();
		auto& sparse_variable_index_to_dense_variable_index_map = this->GetSparseVariableIndexToDenseVariableIndexMap();

		e1_v1_x_sparse_index_ = edge_pair_data_provider.GetEdge1Vertex1XIndex();
		e1_v1_y_sparse_index_ = edge_pair_data_provider.GetEdge1Vertex1YIndex();
		e1_v2_x_sparse_index_ = edge_pair_data_provider.GetEdge1Vertex2XIndex();
		e1_v2_y_sparse_index_ = edge_pair_data_provider.GetEdge1Vertex2YIndex();
		e2_v1_x_sparse_index_ = edge_pair_data_provider.GetEdge2Vertex1XIndex();
		e2_v1_y_sparse_index_ = edge_pair_data_provider.GetEdge2Vertex1YIndex();
		e2_v2_x_sparse_index_ = edge_pair_data_provider.GetEdge2Vertex2XIndex();
		e2_v2_y_sparse_index_ = edge_pair_data_provider.GetEdge2Vertex2YIndex();

		e1_v1_x_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e1_v1_x_sparse_index_);
		e1_v1_y_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e1_v1_y_sparse_index_);
		e1_v2_x_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e1_v2_x_sparse_index_);
		e1_v2_y_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e1_v2_y_sparse_index_);
		e2_v1_x_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e2_v1_x_sparse_index_);
		e2_v1_y_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e2_v1_y_sparse_index_);
		e2_v2_x_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e2_v2_x_sparse_index_);
		e2_v2_y_dense_index_ = sparse_variable_index_to_dense_variable_index_map.at(e2_v2_y_sparse_index_);
		
		dense_index_to_first_derivative_sign_map_.resize(this->objective_variables_count_);
		InitializeDenseIndexToFirstDerivativeSignMap(dense_index_to_first_derivative_sign_map_);

		dense_index_to_first_derivative_value_map_.resize(this->objective_variables_count_);
		dense_indices_to_second_derivative_value_map_.resize(this->objective_variables_count_);
		for (RDS::DenseVariableIndex i = 0; i < this->objective_variables_count_; i++)
		{
			dense_indices_to_second_derivative_value_map_[i].resize(this->objective_variables_count_);
			for (RDS::DenseVariableIndex j = 0; j < this->objective_variables_count_; j++)
			{
				dense_indices_to_second_derivative_value_map_[i][j] = 0;
			}
		}
	}

	/**
	 * Protected fields
	 */
	std::vector<double> dense_index_to_first_derivative_sign_map_;
	std::vector<double> dense_index_to_first_derivative_value_map_;
	std::vector<std::vector<double>> dense_indices_to_second_derivative_value_map_;

	RDS::SparseVariableIndex e1_v1_x_sparse_index_;
	RDS::SparseVariableIndex e1_v1_y_sparse_index_;
	RDS::SparseVariableIndex e1_v2_x_sparse_index_;
	RDS::SparseVariableIndex e1_v2_y_sparse_index_;
	RDS::SparseVariableIndex e2_v1_x_sparse_index_;
	RDS::SparseVariableIndex e2_v1_y_sparse_index_;
	RDS::SparseVariableIndex e2_v2_x_sparse_index_;
	RDS::SparseVariableIndex e2_v2_y_sparse_index_;

	RDS::DenseVariableIndex e1_v1_x_dense_index_;
	RDS::DenseVariableIndex e1_v1_y_dense_index_;
	RDS::DenseVariableIndex e1_v2_x_dense_index_;
	RDS::DenseVariableIndex e1_v2_y_dense_index_;
	RDS::DenseVariableIndex e2_v1_x_dense_index_;
	RDS::DenseVariableIndex e2_v1_y_dense_index_;
	RDS::DenseVariableIndex e2_v2_x_dense_index_;
	RDS::DenseVariableIndex e2_v2_y_dense_index_;

private:
	/**
	 * Private overrides
	 */
	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		auto& edge_pair_data_provider = GetEdgePairDataProvider();
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge1Vertex1XIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge1Vertex1YIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge1Vertex2XIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge1Vertex2YIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge2Vertex1XIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge2Vertex1YIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge2Vertex2XIndex());
		sparse_variable_indices.push_back(edge_pair_data_provider.GetEdge2Vertex2YIndex());
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		auto objective_variable_count = this->objective_variables_count_;
		auto dense_variable_index_to_sparse_variable_index_map = this->GetDenseVariableIndexToSparseVariableIndexMap();
		for (RDS::DenseVariableIndex dense_variable_index = 0; dense_variable_index < objective_variable_count; dense_variable_index++)
		{
			const RDS::SparseVariableIndex sparse_variable_index = dense_variable_index_to_sparse_variable_index_map.at(dense_variable_index);
			g.coeffRef(sparse_variable_index) = CalculateFirstPartialDerivative(dense_variable_index);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const auto triplets_count = triplets.size();
		auto sparse_variable_index_to_dense_variable_index_map = this->GetSparseVariableIndexToDenseVariableIndexMap();
		for (RDS::HessianTripletIndex i = 0; i < triplets_count; i++)
		{
			auto dense_variable_index1 = sparse_variable_index_to_dense_variable_index_map[triplets[i].row()];
			auto dense_variable_index2 = sparse_variable_index_to_dense_variable_index_map[triplets[i].col()];
			const_cast<double&>(triplets[i].value()) = CalculateSecondPartialDerivative(dense_variable_index1, dense_variable_index2);
		}
	}

	/**
	 * Private methods
	 */
	virtual void InitializeDenseIndexToFirstDerivativeSignMap(std::vector<double>& dense_index_to_first_derivative_sign_map) = 0;
	
	double CalculateFirstPartialDerivative(const RDS::SparseVariableIndex dense_variable_index)
	{
		return dense_index_to_first_derivative_sign_map_[dense_variable_index] * dense_index_to_first_derivative_value_map_[dense_variable_index];
	}

	double CalculateSecondPartialDerivative(const RDS::SparseVariableIndex dense_variable_index1, const RDS::SparseVariableIndex dense_variable_index2)
	{
		return dense_index_to_first_derivative_sign_map_[dense_variable_index1] * dense_indices_to_second_derivative_value_map_[dense_variable_index1][dense_variable_index2];
	}
};

#endif