#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// STL includes
#include <utility>
#include <cmath>
#include <algorithm>
#include <unordered_map>

// Eigen includes
#include <Eigen/Eigenvalues> 

// Optimization lib includes
#include "../../utils/type_definitions.h"
#include "../../utils/utils.h"
#include "./utils/data_providers/edge_pair_data_provider.h"
#include "../sparse_objective_function.h"


template<Eigen::StorageOptions StorageOrder_>
class EdgePairObjective : public SparseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Constructors and destructor
	 */
	EdgePairObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::string& name, const RDS::EdgePairDescriptor& edge_pair_descriptor) :
		SparseObjectiveFunction(mesh_data_provider, name, 4, false),
		edge_pair_descriptor_(edge_pair_descriptor)
	{

	}

	virtual ~EdgePairObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreInitialize() override
	{
		SparseObjectiveFunction<StorageOrder_>::PreInitialize();

		edge1_v1_x_index_ = this->mesh_data_provider_->GetVertexXIndex(edge_pair_descriptor_.first.first);
		edge1_v1_y_index_ = this->mesh_data_provider_->GetVertexYIndex(edge_pair_descriptor_.first.first);
		edge1_v2_x_index_ = this->mesh_data_provider_->GetVertexXIndex(edge_pair_descriptor_.first.second);
		edge1_v2_y_index_ = this->mesh_data_provider_->GetVertexYIndex(edge_pair_descriptor_.first.second);

		edge2_v1_x_index_ = this->mesh_data_provider_->GetVertexXIndex(edge_pair_descriptor_.second.first);
		edge2_v1_y_index_ = this->mesh_data_provider_->GetVertexYIndex(edge_pair_descriptor_.second.first);
		edge2_v2_x_index_ = this->mesh_data_provider_->GetVertexXIndex(edge_pair_descriptor_.second.second);
		edge2_v2_y_index_ = this->mesh_data_provider_->GetVertexYIndex(edge_pair_descriptor_.second.second);

		//partial_to_sparse_index_map_.insert({ Partial::E1_V1_X, edge1_v1_x_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E1_V1_Y, edge1_v1_y_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E1_V2_X, edge1_v2_x_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E1_V2_Y, edge1_v2_y_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E2_V1_X, edge2_v1_x_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E2_V1_Y, edge2_v1_y_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E2_V2_X, edge2_v2_x_index_ });
		//partial_to_sparse_index_map_.insert({ Partial::E2_V2_Y, edge2_v2_y_index_ });

		//sparse_index_to_partial_map_.insert({ edge1_v1_x_index_, Partial::E1_V1_X });
		//sparse_index_to_partial_map_.insert({ edge1_v1_y_index_, Partial::E1_V1_Y });
		//sparse_index_to_partial_map_.insert({ edge1_v2_x_index_, Partial::E1_V2_X });
		//sparse_index_to_partial_map_.insert({ edge1_v2_y_index_, Partial::E1_V2_Y });
		//sparse_index_to_partial_map_.insert({ edge2_v1_x_index_, Partial::E2_V1_X });
		//sparse_index_to_partial_map_.insert({ edge2_v1_y_index_, Partial::E2_V1_Y });
		//sparse_index_to_partial_map_.insert({ edge2_v2_x_index_, Partial::E2_V2_X });
		//sparse_index_to_partial_map_.insert({ edge2_v2_y_index_, Partial::E2_V2_Y });

		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E1_V1_X,  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E1_V1_Y, -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E1_V2_X, -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E1_V2_Y,  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E2_V1_X, -1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E2_V1_Y,  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E2_V2_X,  1 });
		sparse_index_to_first_derivative_sign_map_.insert({ Partial::E2_V2_Y, -1 });

		partial_to_partial_coordinate_type_.insert({ Partial::E1_V1_X, PartialCoordinateType::E1_X });
		partial_to_partial_coordinate_type_.insert({ Partial::E1_V1_Y, PartialCoordinateType::E1_Y });
		partial_to_partial_coordinate_type_.insert({ Partial::E1_V2_X, PartialCoordinateType::E1_X });
		partial_to_partial_coordinate_type_.insert({ Partial::E1_V2_Y, PartialCoordinateType::E1_Y });
		partial_to_partial_coordinate_type_.insert({ Partial::E2_V1_X, PartialCoordinateType::E2_X });
		partial_to_partial_coordinate_type_.insert({ Partial::E2_V1_Y, PartialCoordinateType::E2_Y });
		partial_to_partial_coordinate_type_.insert({ Partial::E2_V2_X, PartialCoordinateType::E2_X });
		partial_to_partial_coordinate_type_.insert({ Partial::E2_V2_Y, PartialCoordinateType::E2_Y });

		for (int i = 0; i < this->objective_variable_count_; i++)
		{
			auto partial = static_cast<Partial>(i);
			sparse_index_to_first_derivative_value_map_.insert({ partial, 0 });
		}

		for (int i = 0; i < this->objective_vertices_count_; i++)
		{
			for (int j = 0; j < this->objective_variable_count_; j++)
			{
				//PartialCoordinateType first_partial_type = static_cast<PartialCoordinateType>(i);
				//Partial second_partial = static_cast<Partial>(j);
				vertex_index_sparse_index_to_second_derivative_value_map_.insert({ std::make_pair(first_partial_type, second_partial), 0 });
			}
		}
	}

	void PostInitialize() override
	{
		SparseObjectiveFunction<StorageOrder_>::PostInitialize();

		//for (std::size_t i = 0; i < sparse.size(); i++)
		//{
		//	partial_to_dense_index_map_.insert({ sparse_index_to_partial_map_[indices_.at(i)], i });
		//}
	}

private:
	/**
	 * Private type declarations
	 */
	//enum class Partial
	//{
	//	E1_V1_X,
	//	E1_V1_Y,
	//	E1_V2_X,
	//	E1_V2_Y,
	//	E2_V1_X,
	//	E2_V1_Y,
	//	E2_V2_X,
	//	E2_V2_Y
	//};

	//enum class PartialCoordinateType
	//{
	//	E1_X,
	//	E1_Y,
	//	E2_X,
	//	E2_Y
	//};

	/**
	 * Private overrides
	 */
	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		sparse_variable_indices.push_back(edge1_v1_x_index_);
		sparse_variable_indices.push_back(edge1_v1_y_index_);
		sparse_variable_indices.push_back(edge1_v2_x_index_);
		sparse_variable_indices.push_back(edge1_v2_y_index_);
		sparse_variable_indices.push_back(edge2_v1_x_index_);
		sparse_variable_indices.push_back(edge2_v1_y_index_);
		sparse_variable_indices.push_back(edge2_v2_x_index_);
		sparse_variable_indices.push_back(edge2_v2_y_index_);
	}

	void CalculateValue(double& f) override
	{
		f = std::atan2(edge1_y_diff_, edge1_x_diff_) - atan2(edge2_y_diff_, edge2_x_diff_);
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
		for (int i = 0; i < 8; i++)
		{
			Partial partial = static_cast<Partial>(i);
			g.coeffRef(partial_to_sparse_index_map_[partial]) = CalculateFirstPartialDerivative(partial);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const std::size_t triplets_count = triplets.size();
		for (int64_t i = 0; i < triplets_count; i++)
		{
			auto first_partial = sparse_index_to_partial_map_[triplets[i].col()];
			auto second_partial = sparse_index_to_partial_map_[triplets[i].row()];
			const_cast<double&>(triplets[i].value()) = CalculateSecondPartialDerivative(triplets[i].col(), triplets[i].row());
		}
	}

	/**
	 * Private methods
	 */
	double CalculateFirstPartialDerivative(const RDS::SparseVariableIndex partial)
	{
		return sparse_index_to_first_derivative_sign_map_[partial] * sparse_index_to_first_derivative_value_map_[partial];
	}


	double CalculateSecondPartialDerivative(const RDS::SparseVariableIndex first_partial, const RDS::SparseVariableIndex second_partial)
	{
		return sparse_index_to_first_derivative_sign_map_[first_partial] * vertex_index_sparse_index_to_second_derivative_value_map_[{ partial_to_partial_coordinate_type_[first_partial], second_partial }];
	}

	/**
	 * Private fields
	 */
	RDS::EdgePairDescriptor edge_pair_descriptor_;

	RDS::SparseVariableIndex edge1_v1_x_index_;
	RDS::SparseVariableIndex edge1_v1_y_index_;
	RDS::SparseVariableIndex edge1_v2_x_index_;
	RDS::SparseVariableIndex edge1_v2_y_index_;

	RDS::SparseVariableIndex edge2_v1_x_index_;
	RDS::SparseVariableIndex edge2_v1_y_index_;
	RDS::SparseVariableIndex edge2_v2_x_index_;
	RDS::SparseVariableIndex edge2_v2_y_index_;

	//std::unordered_map<Partial, PartialCoordinateType> partial_to_partial_coordinate_type_;
	//std::unordered_map<Partial, int64_t> partial_to_sparse_index_map_;
	//std::unordered_map<int64_t, Partial> sparse_index_to_partial_map_;
	//std::unordered_map<Partial, int64_t> partial_to_dense_index_map_;
	std::unordered_map<RDS::SparseVariableIndex, double> sparse_index_to_first_derivative_sign_map_;
	std::unordered_map<RDS::SparseVariableIndex, double> sparse_index_to_first_derivative_value_map_;
	std::unordered_map<std::pair<RDS::VertexIndex, RDS::SparseVariableIndex>, double, RDS::OrderedPairHash, RDS::OrderedPairEquals> vertex_index_sparse_index_to_second_derivative_value_map_;
};

#endif