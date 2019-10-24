#pragma once
#ifndef OPTIMIZATION_LIB_SPARSE_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_SPARSE_OBJECTIVE_FUNCTION_H

// Optimization lib includes
#include "./objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class SparseObjectiveFunction : public ObjectiveFunction<StorageOrder_, Eigen::SparseVector<double>>
{
public:
	/**
	 * Constructors and destructor
	 */
	SparseObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		ObjectiveFunction(objective_function_data_provider, name)
	{

	}

	virtual ~SparseObjectiveFunction()
	{
		// Empty implementation
	}
};

#endif