#pragma once
#ifndef OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H
#define OPTIMIZATION_LIB_EDGE_PAIR_ANGLE_OBJECTIVE_H

// Optimization lib includes
#include "./periodic_objective.h"

template<Eigen::StorageOptions StorageOrder_>
class EdgePairAngleObjective : public PeriodicObjective<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : uint32_t
	{
		Period = SparseObjectiveFunction<StorageOrder_>::Properties::Count_
	};

	/**
	 * Constructors and destructor
	 */
	EdgePairAngleObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double period) :
		PeriodicObjective(objective_function_data_provider, "Edge Pair Angle Objective")
	{
		this->Initialize();
	}

	virtual ~EdgePairAngleObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void CalculateValueInner(double& f) override
	{
		
	}
	
	void CalculateGradientInner(GradientType_& g) override
	{
		
	}
	
	void CalculateHessianInner(Eigen::SparseMatrix<double, StorageOrder_>& H) override
	{
		
	}
};

#endif