#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./dense_objective_function.h"

template<typename ObjectiveFunctionType_>
class CompositeObjective : public DenseObjectiveFunction<static_cast<Eigen::StorageOptions>(ObjectiveFunctionType_::StorageOrder)>
{
public:
	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		DenseObjectiveFunction(objective_function_data_provider, "Composite Objective"),
		explicitly_zero_diagonal_(explicitly_zero_diagonal),
		parallel_update_(parallel_update)
	{
		this->Initialize();
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		DenseObjectiveFunction(objective_function_data_provider, name),
		explicitly_zero_diagonal_(explicitly_zero_diagonal),
		parallel_update_(parallel_update)
	{
		this->Initialize();
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const std::string& name, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		CompositeObjective(objective_function_data_provider, name, explicitly_zero_diagonal, parallel_update)
	{
		AddObjectiveFunctions(objective_functions);
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const std::string& name, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		CompositeObjective(objective_function_data_provider, std::vector<std::shared_ptr<ObjectiveFunctionType_>>{ objective_function }, name, explicitly_zero_diagonal, parallel_update)
	{

	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		CompositeObjective(objective_function_data_provider, objective_functions, "Composite Objective", explicitly_zero_diagonal, parallel_update)
	{

	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const bool explicitly_zero_diagonal = false, const bool parallel_update = false) :
		CompositeObjective(objective_function_data_provider, objective_function, "Composite Objective", explicitly_zero_diagonal, parallel_update)
	{

	}

	virtual ~CompositeObjective()
	{

	}

	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunctionType_>& objective_function)
	{
		std::lock_guard<std::mutex> lock(m_);
		objective_functions_.push_back(objective_function);
	}

	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		std::lock_guard<std::mutex> lock(m_);
		for (auto& objective_function : objective_functions)
		{
			objective_functions_.push_back(objective_function);
		}
	}

	void RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunctionType_>& objective_function)
	{
		std::lock_guard<std::mutex> lock(m_);
		objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
	}

	void RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		std::lock_guard<std::mutex> lock(m_);
		for (const auto& objective_function : objective_functions)
		{
			objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
		}
	}

	std::uint32_t GetObjectiveFunctionsCount() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return objective_functions_.size();
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(std::uint32_t index) const
	{
		std::lock_guard<std::mutex> lock(m_);
		if (index < objective_functions_.size())
		{
			return objective_functions_.at(index);
		}

		return nullptr;
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(const std::string& name) const
	{
		std::lock_guard<std::mutex> lock(m_);
		for (auto& objective_function : objective_functions_)
		{
			if (!objective_function->GetName().compare(name))
			{
				return objective_function;
			}
		}

		return nullptr;
	}

private:

	/**
	 * Private overrides
	 */
	void CalculateValue(double& f) override
	{
		f = 0;
		for (const auto& objective_function : objective_functions_)
		{
			auto w = objective_function->GetWeight();
			f += w * objective_function->GetValue();
		}
	}

	void CalculateValuePerVertex(Eigen::VectorXd& f_per_vertex) override
	{
		f_per_vertex.setZero();
		#pragma omp parallel for if(parallel_update_)
		for (int64_t i = 0; i < objective_functions_.size(); i++)
		{
			auto& objective_function = objective_functions_.at(i);
			auto w = objective_function->GetWeight();
			objective_function->AddValuePerVertex(f_per_vertex, w);
		}
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		g.setZero();
		//#pragma omp parallel for if(parallel_update_)
		//for (const auto& objective_function : objective_functions_)

		#pragma omp parallel for if(parallel_update_)
		for(int64_t i = 0; i < objective_functions_.size(); i++)
		{
			auto& objective_function = objective_functions_.at(i);
			auto w = objective_function->GetWeight();
			objective_function->AddGradient(g, w);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.clear();
		for (const auto& objective_function : objective_functions_)
		{
			auto w = objective_function->GetWeight();
			objective_function->AddTriplets(triplets, w);
		}
	}

	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		
	}

	void PreInitialize() override
	{
		for (const auto& objective_function : objective_functions_)
		{
			objective_function->Initialize();
		}
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		#pragma omp parallel for if(parallel_update_)
		for(int32_t i = 0; i < objective_functions_.size(); i++)
		{
			objective_functions_[i]->Update(x);
		}
	}

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunctionType_>> objective_functions_;
	bool explicitly_zero_diagonal_;
	bool parallel_update_;
};

#endif