#pragma once
#ifndef OPTIMIZATION_LIB_INTEGER_OBJECTIVE_H
#define OPTIMIZATION_LIB_INTEGER_OBJECTIVE_H

// Optimization lib includes
#include "./periodic_objective.h"
#include "../utils/utils.h"

template<Eigen::StorageOptions StorageOrder_>
class IntegerObjective : public PeriodicObjective<StorageOrder_>
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
	IntegerObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const uint64_t index) :
		PeriodicObjective(objective_function_data_provider, "Integer Objective", 1, false),
		index_(index)
	{
		this->Initialize();
	}

	virtual ~IntegerObjective()
	{

	}

protected:
	/**
	 * Protected overrides
	 */
	void CalculateValueInner(double& f) override
	{
		f = value_;
	}

	void CalculateGradientInner(Eigen::SparseVector<double>& g) override
	{
		g.coeffRef(index_) = 1;
	}

	void CalculateTripletsInner(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.resize(1);
		triplets[0] = Eigen::Triplet<double>(index_, index_, 0);
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		value_ = x.coeffRef(index_);
	}

private:
	/**
	 * Private overrides
	 */
	void CalculateValuePerVertex(Eigen::SparseVector<double>& f_per_vertex) override
	{

	}

	/**
	 * Private overrides
	 */
	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{

	}

	/**
	 * Private fields
	 */
	uint64_t index_;
	double value_;
};

#endif