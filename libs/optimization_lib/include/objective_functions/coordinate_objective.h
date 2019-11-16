#pragma once
#ifndef OPTIMIZATION_LIB_COORDINATE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COORDINATE_OBJECTIVE_H

// Optimization lib includes
#include "../utils/type_definitions.h"
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

	enum class CoordinateType : int32_t
	{
		X,
		Y
	};

	/**
	 * Constructors and destructor
	 */
	CoordinateObjective(const std::shared_ptr<PlainDataProvider>& plain_data_provider, const RDS::VertexIndex vertex_index, CoordinateType coordinate_type) :
		SparseObjectiveFunction(plain_data_provider, "Integer Objective", 1, 1, false),
		vertex_index_(vertex_index),
		coordinate_type_(coordinate_type)
	{
		this->Initialize();
	}

	virtual ~CoordinateObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void PreInitialize() override
	{
		switch (coordinate_type_)
		{
		case CoordinateType::X:
			sparse_variable_index_ = this->data_provider_->GetMeshDataProvider().GetVertexXIndex(vertex_index_);
			break;
		case CoordinateType::Y:
			sparse_variable_index_ = this->data_provider_->GetMeshDataProvider().GetVertexYIndex(vertex_index_);
			break;
		}
	}
	
	void CalculateValue(double& f) override
	{
		f = coordinate_value_;
	}

	void CalculateGradient(Eigen::SparseVector<double>& g) override
	{
		g.coeffRef(sparse_variable_index_) = 1;
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets[0] = Eigen::Triplet<double>(sparse_variable_index_, sparse_variable_index_, 0);
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		coordinate_value_ = x.coeffRef(sparse_variable_index_);
	}

private:
	/**
	 * Private overrides
	 */

	void InitializeSparseVariableIndices(std::vector<RDS::SparseVariableIndex>& sparse_variable_indices) override
	{
		sparse_variable_indices.push_back(sparse_variable_index_);
	}

	/**
	 * Private fields
	 */
	RDS::VertexIndex vertex_index_;
	RDS::SparseVariableIndex sparse_variable_index_;
	CoordinateType coordinate_type_;
	double coordinate_value_;
};

#endif