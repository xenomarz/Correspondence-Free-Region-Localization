#pragma once
#ifndef OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H
#define OPTIMIZATION_LIB_CONCRETE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "../utils/data_providers/mesh_data_provider.h"
#include "./dense_objective_function.h"

template<typename ObjectiveFunctionType_>
class ConcreteObjective : public ObjectiveFunctionType_
{
public:
	/**
	 * Constructors and destructor
	 */
	ConcreteObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::string& name) :
		ObjectiveFunctionType_(mesh_data_provider, name)
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	virtual void PostInitialize() override
	{
		InitializeTriplets(triplets_);
	}
	
private:
	/**
	 * Private overrides
	 */
	void CalculateHessian(Eigen::SparseMatrix<double, ObjectiveFunctionType_::StorageOrder>& H) override
	{
		CalculateTriplets(triplets_);
		H.setFromTriplets(triplets_.begin(), triplets_.end());
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