#pragma once
#ifndef OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H
#define OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H

// STL Includes
#include <memory>
#include <atomic>
#include <string>
#include <mutex>
#include <any>
#include <limits>

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues> 

// Optimization Lib Includes
#include "./objective_function.h"
#include "../core/core.h"
#include "../data_providers/data_provider.h"

template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
class ConcreteObjective : public ObjectiveFunction<StorageOrder_, VectorType_>
{
public:
	/**
	 * Constructor and destructor
	 */
	ConcreteObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const bool enforce_psd) :
		ConcreteObjective(mesh_data_provider, data_provider, name, objective_vertices_count, 2 * objective_vertices_count, enforce_psd)
	{
		
	}

	ConcreteObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const int64_t objective_variables_count, const bool enforce_psd) :
		ObjectiveFunction(mesh_data_provider, data_provider, name),
		objective_vertices_count_(objective_vertices_count),
		objective_variables_count_(objective_variables_count),
		enforce_psd_(enforce_psd)
	{

	}

	virtual ~ConcreteObjective()
	{
		// Empty implementation
	}

	/**
	 * Getters
	 */

	int64_t GetObjectiveVerticesCount() const
	{
		return objective_vertices_count_;
	}

	int64_t GetObjectiveVariablesCount() const
	{
		return objective_variables_count_;
	}

	bool GetEnforcePsd() const
	{
		return enforce_psd_;
	}

	const std::vector<RDS::SparseVariableIndex>& GetSparseVariablesIndices() const
	{
		return sparse_variable_indices_;
	}

	const RDS::HessianEntryToTripletIndexMap& GetHessianEntryToTripletIndexMap() const
	{
		return hessian_entry_to_triplet_index_map_;
	}

	const RDS::SparseVariableIndexToDenseVariableIndexMap& GetSparseVariableIndexToDenseVariableIndexMap() const
	{
		return sparse_variable_index_to_dense_variable_index_map_;
	}

	const RDS::DenseVariableIndexToSparseVariableIndexMap& GetDenseVariableIndexToSparseVariableIndexMap() const
	{
		return dense_variable_index_to_sparse_variable_index_map_;
	}

	/**
	 * Setters
	 */
	void SetEnforcePsd(const bool enforce_psd)
	{
		enforce_psd_ = enforce_psd;
	}

protected:
	/**
	 * Protected Fields
	 */
	
	// Mutex
	mutable std::mutex m_;

	// Elements count
	int64_t objective_vertices_count_;
	int64_t objective_variables_count_;

	// Parallelism enabled flag
	bool parallelism_enabled_;
	
private:

	/**
	 * Private methods
	 */
	virtual void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices)
	{
		// Empty implementation
	}

	virtual void InitializeMappings(
		RDS::SparseVariableIndexToDenseVariableIndexMap& sparse_variable_index_to_dense_variable_index_map,
		RDS::DenseVariableIndexToSparseVariableIndexMap& dense_variable_index_to_sparse_variable_index_map,
		RDS::SparseVariableIndexToVertexIndexMap& sparse_variable_index_to_vertex_index_map)
	{
		std::sort(sparse_variable_indices_.begin(), sparse_variable_indices_.end());
		for (std::size_t i = 0; i < sparse_variable_indices_.size(); i++)
		{
			dense_variable_index_to_sparse_variable_index_map.insert({ i, sparse_variable_indices_[i] });
			sparse_variable_index_to_dense_variable_index_map.insert({ sparse_variable_indices_[i], i });
			sparse_variable_index_to_vertex_index_map.insert({ sparse_variable_indices_[i], mesh_data_provider_->GetVertexIndex(sparse_variable_indices_[i])});
		}
	}

	void CreateTriplets(std::vector<Eigen::Triplet<double>>& triplets)
	{
		const auto objective_variables_count_squared = objective_variables_count_ * objective_variables_count_;
		triplets.resize(((objective_variables_count_squared - objective_variables_count_) / 2) + objective_variables_count_);
		auto triplet_index = 0;
		for (auto column = 0; column < objective_variables_count_; column++)
		{
			for (auto row = 0; row <= column; row++)
			{
				triplets[triplet_index] = Eigen::Triplet<double>(
					dense_variable_index_to_sparse_variable_index_map_[row],
					dense_variable_index_to_sparse_variable_index_map_[column],
					0);

				hessian_entry_to_triplet_index_map_[{ row, column }] = triplet_index;
				triplet_index++;
			}
		}
	}

	virtual void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets)
	{
		InitializeSparseVariableIndices(sparse_variable_indices_);
		InitializeMappings(
			sparse_variable_index_to_dense_variable_index_map_,
			dense_variable_index_to_sparse_variable_index_map_,
			sparse_variable_index_to_vertex_index_map_);
		CreateTriplets(triplets);
	}

	// Value, gradient and hessian calculation functions
	virtual void CalculateValuePerVertex(VectorType_& f_per_vertex)
	{
		const double value = this->GetValueInternal();
		f_per_vertex.setZero();
		const auto sparse_variables_indices_count = sparse_variable_indices_.size();
		for (std::size_t i = 0; i < sparse_variables_indices_count; i++)
		{
			f_per_vertex.coeffRef(sparse_variable_index_to_vertex_index_map_[sparse_variable_indices_[i]]) += value;
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets)
	{
		CalculateRawTriplets(triplets);
		CalculateConvexTriplets(triplets);
	}

	virtual void CalculateRawTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;
	
	void CalculateConvexTriplets(std::vector<Eigen::Triplet<double>>& triplets)
	{
		if (enforce_psd_)
		{
			Eigen::MatrixXd H;
			H.resize(objective_variables_count_, objective_variables_count_);
			H.setZero();

			auto triplets_count = triplets.size();
			for (std::size_t i = 0; i < triplets_count; i++)
			{
				auto row = sparse_variable_index_to_dense_variable_index_map_[triplets[i].row()];
				auto col = sparse_variable_index_to_dense_variable_index_map_[triplets[i].col()];
				auto value = triplets[i].value();
				H.coeffRef(row, col) = value;
				H.coeffRef(col, row) = value;
			}

			Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(H);
			Eigen::MatrixXd D = solver.eigenvalues().asDiagonal();
			Eigen::MatrixXd V = solver.eigenvectors();
			for (auto i = 0; i < objective_variables_count_; i++)
			{
				auto& value = D.coeffRef(i, i);
				if (value < 0)
				{
					value = 10e-8;
				}
			}

			H = V * D * V.transpose();

			for (auto column = 0; column < objective_variables_count_; column++)
			{
				for (auto row = 0; row <= column; row++)
				{
					const auto triplet_index = hessian_entry_to_triplet_index_map_[{row, column}];
					const_cast<double&>(triplets[triplet_index].value()) = H.coeffRef(row, column);
				}
			}
		}
	}

	/**
	 * Private fields
	 */

	// Enforce PSD
	bool enforce_psd_;

	// Sparse variable indices
	std::vector<RDS::SparseVariableIndex> sparse_variable_indices_;

	// Mappings
	RDS::HessianEntryToTripletIndexMap hessian_entry_to_triplet_index_map_;
	RDS::SparseVariableIndexToDenseVariableIndexMap sparse_variable_index_to_dense_variable_index_map_;
	RDS::DenseVariableIndexToSparseVariableIndexMap dense_variable_index_to_sparse_variable_index_map_;
	RDS::SparseVariableIndexToVertexIndexMap sparse_variable_index_to_vertex_index_map_;
};

#endif