#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_OBJECTIVE_H

// STL includes
#include <utility>
#include <unordered_map>

// Optimization lib includes
#include "../../utils/type_definitions.h"
#include "../../utils/utils.h"
#include "../../utils/data_providers/edge_pair_data_provider.h"
#include "../sparse_objective_function.h"


template<Eigen::StorageOptions StorageOrder_>
class EdgePairObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider, const std::string& name) :
		SparseObjectiveFunction(mesh_data_provider, edge_pair_data_provider, name, 4, false)
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
	void PreInitialize() override
	{
		SparseObjectiveFunction<StorageOrder_>::PreInitialize();

		auto& edge_pair_data_provider = GetEdgePairDataProvider();
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge1Vertex1XIndex(),  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge1Vertex1YIndex(), -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge1Vertex2XIndex(), -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge1Vertex2YIndex(),  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge2Vertex1XIndex(), -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge2Vertex1YIndex(),  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge2Vertex2XIndex(),  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ edge_pair_data_provider.GetEdge2Vertex2YIndex(), -1 });

		for (RDS::DenseVariableIndex dense_variable_index = 0; dense_variable_index < this->objective_variables_count_; dense_variable_index++)
		{
			const RDS::SparseVariableIndex sparse_variable_index = this->dense_variable_index_to_sparse_variable_index_map_[dense_variable_index];
			sparse_index_to_first_derivative_value_map_.insert({ sparse_variable_index, 0 });
		}

		for (RDS::DenseVariableIndex dense_variable_index1 = 0; dense_variable_index1 < this->objective_variable_count_; dense_variable_index1++)
		{
			for (RDS::DenseVariableIndex dense_variable_index2 = 0; dense_variable_index2 < this->objective_variable_count_; dense_variable_index2++)
			{
				const RDS::SparseVariableIndex sparse_variable_index1 = this->dense_variable_index_to_sparse_variable_index_map_[dense_variable_index1];
				const RDS::SparseVariableIndex sparse_variable_index2 = this->dense_variable_index_to_sparse_variable_index_map_[dense_variable_index2];
				sparse_indices_to_second_derivative_value_map_.insert({ std::make_pair(sparse_variable_index1, sparse_variable_index2), 0 });
			}
		}
	}

	/**
	 * Protected fields
	 */
	std::unordered_map<RDS::SparseVariableIndex, double> sparse_index_to_first_derivative_sign_map_;
	std::unordered_map<RDS::SparseVariableIndex, double> sparse_index_to_first_derivative_value_map_;
	std::unordered_map<std::pair<RDS::SparseVariableIndex, RDS::SparseVariableIndex>, double, RDS::OrderedPairHash, RDS::OrderedPairEquals> sparse_indices_to_second_derivative_value_map_;

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

	void CalculateValuePerVertex(Eigen::SparseVector<double>& f_per_vertex) override
	{
		//double value = this->GetValueInternal();
		//f_per_vertex.setZero();
		//for(int i = 0; i < indices_.size(); i++)
		//{
		//	f_per_vertex.coeffRef(indices_[i]) += value;
		//}
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		auto objective_variables_count = this->objective_variables_count_;
		for (RDS::DenseVariableIndex dense_variable_index = 0; dense_variable_index < objective_variables_count; dense_variable_index++)
		{
			const RDS::SparseVariableIndex sparse_variable_index = this->dense_variable_index_to_sparse_variable_index_map_[dense_variable_index];
			g.coeffRef(sparse_variable_index) = CalculateFirstPartialDerivative(sparse_variable_index);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const auto triplets_count = triplets.size();
		for (RDS::HessianTripletIndex i = 0; i < triplets_count; i++)
		{
			const_cast<double&>(triplets[i].value()) = CalculateSecondPartialDerivative(triplets[i].col(), triplets[i].row());
		}
	}

	/**
	 * Private methods
	 */
	double CalculateFirstPartialDerivative(const RDS::SparseVariableIndex sparse_variable_index)
	{
		return sparse_index_to_first_derivative_sign_map_[sparse_variable_index] * sparse_index_to_first_derivative_value_map_[sparse_variable_index];
	}


	double CalculateSecondPartialDerivative(const RDS::SparseVariableIndex sparse_variable_index1, const RDS::SparseVariableIndex sparse_variable_index2)
	{
		return sparse_index_to_first_derivative_sign_map_[sparse_variable_index1] * sparse_indices_to_second_derivative_value_map_[{ sparse_variable_index1, sparse_variable_index2 }];
	}
};

#endif