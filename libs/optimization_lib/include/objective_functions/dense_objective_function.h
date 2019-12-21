#pragma once
#ifndef OPTIMIZATION_LIB_DENSE_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_DENSE_OBJECTIVE_FUNCTION_H

// Optimization lib includes
#include "./concrete_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class DenseObjectiveFunction : public ConcreteObjective<StorageOrder_, Eigen::VectorXd>
{
public:
	/**
	 * Constructors and destructor
	 */
	DenseObjectiveFunction(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const int64_t objective_variables_count, const bool enforce_psd) :
		ConcreteObjective(mesh_data_provider, data_provider, name, objective_vertices_count, objective_variables_count, enforce_psd)
	{

	}

	DenseObjectiveFunction(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const bool enforce_psd) :
		ConcreteObjective(mesh_data_provider, data_provider, name, objective_vertices_count, enforce_psd)
	{

	}

	virtual ~DenseObjectiveFunction()
	{
		// Empty implementation
	}
};

#endif