#pragma once
#ifndef OPTIMIZATION_LIB_REGION_LOCALIZATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_REGION_LOCALIZATION_OBJECTIVE_H

// STL includes
#include <vector>

// Optimization lib includes
#include "../data_providers/plain_data_provider.h"
#include "./dense_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class RegionLocalizationObjective : public DenseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Delta = DenseObjectiveFunction<StorageOrder_>::Properties::Count_,
		ValuePerEdge
	};
	
	/**
	 * Constructors and destructor
	 */
	RegionLocalizationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EmptyDataProvider>& empty_data_provider) :
		DenseObjectiveFunction(mesh_data_provider, empty_data_provider, "Region Localization", 0, false)
	{
		this->Initialize();
	}

	virtual ~RegionLocalizationObjective()
	{

	}

	/**
	 * Setters
	 */

	/**
	 * Getters
	 */


private:

	/**
	 * Overrides
	 */
	void CalculateValue(double& f) override
	{

	}

	void CalculateValuePerVertex(Eigen::VectorXd& f_per_vertex) override
	{

	}

	void CalculateValuePerEdge(Eigen::VectorXd& domain_value_per_edge, Eigen::VectorXd& image_value_per_edge) override
	{

	}
	
	void CalculateGradient(Eigen::VectorXd& g) override
	{

	}
	
	void PreUpdate(const Eigen::VectorXd& x) override
	{

	}
	
	void PreInitialize() override
	{

	}
	
	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{

	}
	
	void CalculateRawTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{

	}
	
	/**
	 * Private methods
	 */

	/**
	 * Fields
	 */
};

#endif