#pragma once
#ifndef OPTIMIZATION_LIB_DENSE_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_DENSE_OBJECTIVE_FUNCTION_H

// Optimization lib includes
#include "./objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class DenseObjectiveFunction : public ObjectiveFunction<StorageOrder_, Eigen::VectorXd>
{
public:
	/**
	 * Constructors and destructor
	 */
	DenseObjectiveFunction(const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const int64_t objective_vertices_count, const bool enforce_psd) :
		ObjectiveFunction(data_provider, name, objective_vertices_count, enforce_psd)
	{

	}

	virtual ~DenseObjectiveFunction()
	{
		// Empty implementation
	}
};

#endif