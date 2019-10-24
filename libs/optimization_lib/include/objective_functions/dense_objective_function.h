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
	DenseObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		ObjectiveFunction(objective_function_data_provider, name)
	{

	}

	virtual ~DenseObjectiveFunction()
	{
		// Empty implementation
	}
};

#endif