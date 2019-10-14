#pragma once
#ifndef OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H
#define OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./objective_function.h"

template<Eigen::StorageOptions StorageOrder>
class ConcreteObjective : public ObjectiveFunction<StorageOrder>
{
public:
	/**
	 * Constructors and destructor
	 */
	ConcreteObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		ObjectiveFunction(objective_function_data_provider, name)
	{

	}

private:

	/**
	 * Private overrides
	 */
	void CalculateHessian(Eigen::SparseMatrix<double, StorageOrder>& H) override
	{
		CalculateTriplets(triplets_);
		H.setFromTriplets(triplets_.begin(), triplets_.end());
	}

	void PostInitialize() override
	{
		InitializeTriplets(triplets_);
	}

	/**
	 * Private methods
	 */
	virtual void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;
	virtual void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;
	
	/**
	 * Private fields
	 */
	std::vector<Eigen::Triplet<double>> triplets_;
};

#endif