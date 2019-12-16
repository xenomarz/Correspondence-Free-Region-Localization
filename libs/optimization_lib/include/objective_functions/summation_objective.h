#pragma once
#ifndef OPTIMIZATION_LIB_SUMMATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_SUMMATION_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./objective_function.h"

template<typename ObjectiveFunctionType_, typename VectorType_>
class SummationObjective : public ObjectiveFunction<static_cast<Eigen::StorageOptions>(ObjectiveFunctionType_::StorageOrder), VectorType_>
{
public:
	/**
	 * Constructors and destructor
	 */
	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		ObjectiveFunction(data_provider, "Summation Objective", 0, enforce_psd),
		parallel_update_(parallel_update),
		enforce_children_psd_(enforce_children_psd)
	{
		this->parallelism_enabled_ = true;
		this->Initialize();
	}

	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		ObjectiveFunction(data_provider, name, 0, enforce_psd),
		parallel_update_(parallel_update),
		enforce_children_psd_(enforce_children_psd)
	{
		this->parallelism_enabled_ = true;
		this->Initialize();
	}

	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const std::string& name, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		SummationObjective(data_provider, name, enforce_psd, enforce_children_psd, parallel_update)
	{
		AddObjectiveFunctions(objective_functions);
	}

	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const std::string& name, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		SummationObjective(data_provider, std::vector<std::shared_ptr<ObjectiveFunctionType_>>{ objective_function }, name, enforce_psd, enforce_children_psd, parallel_update)
	{

	}

	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		SummationObjective(data_provider, objective_functions, "Summation Objective", enforce_psd, enforce_children_psd, parallel_update)
	{

	}

	SummationObjective(const std::shared_ptr<DataProvider>& data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const bool enforce_psd = false, const bool enforce_children_psd = false, const bool parallel_update = false) :
		SummationObjective(data_provider, objective_function, "Summation Objective", enforce_psd, enforce_children_psd, parallel_update)
	{

	}

	virtual ~SummationObjective()
	{

	}

	/**
	 * Public getters
	 */
	bool GetEnforceChildrenPsd() const
	{
		return enforce_children_psd_;
	}

	/**
	 * Public setters
	 */
	void SetEnforceChildrenPsd(const bool enforce_children_psd)
	{
		enforce_children_psd_ = enforce_children_psd;
	}
	
	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunctionType_>& objective_function)
	{
		//std::lock_guard<std::mutex> lock(m_);
		objective_functions_.push_back(objective_function);
	}

	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		//std::lock_guard<std::mutex> lock(m_);
		for (auto& objective_function : objective_functions)
		{
			objective_functions_.push_back(objective_function);
		}
	}

	void RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunctionType_>& objective_function)
	{
		//std::lock_guard<std::mutex> lock(m_);
		objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
	}

	void RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		//std::lock_guard<std::mutex> lock(m_);
		for (const auto& objective_function : objective_functions)
		{
			objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
		}
	}

	std::uint32_t GetObjectiveFunctionsCount() const
	{
		//std::lock_guard<std::mutex> lock(m_);
		return GetObjectiveFunctionsCountInternal();
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(std::uint32_t index) const
	{
		//std::lock_guard<std::mutex> lock(m_);
		return GetObjectiveFunctionInternal(index);
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(const std::string& name) const
	{
		//std::lock_guard<std::mutex> lock(m_);
		return GetObjectiveFunctionInternal(name);
	}

protected:
	/**
	 * Protected getters
	 */
	std::uint32_t GetObjectiveFunctionsCountInternal() const
	{
		return objective_functions_.size();
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunctionInternal(std::uint32_t index) const
	{
		if (index < objective_functions_.size())
		{
			return objective_functions_.at(index);
		}

		return nullptr;
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunctionInternal(const std::string& name) const
	{
		for (auto& objective_function : objective_functions_)
		{
			if (!objective_function->GetName().compare(name))
			{
				return objective_function;
			}
		}

		return nullptr;
	}
	
	/**
	 * Protected overrides
	 */
	void PreInitialize() override
	{
		for (const auto& objective_function : objective_functions_)
		{
			objective_function->Initialize();
		}
	}

	void PreUpdate(const Eigen::VectorXd& x, UpdatableObject::UpdatedObjectSet& updated_objects) override
	{
		auto objective_functions_size = objective_functions_.size();

		for (int32_t i = 0; i < objective_functions_size; i++)
		{
			objective_functions_[i]->Update(x, updated_objects);
		}
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

	void CalculateValuePerVertex(VectorType_& f_per_vertex) override
	{
		f_per_vertex.setZero();
		for (int64_t i = 0; i < objective_functions_.size(); i++)
		{
			auto& objective_function = objective_functions_.at(i);
			auto w = objective_function->GetWeight();
			objective_function->AddValuePerVertex(f_per_vertex, w);
		}
	}

	void CalculateGradient(VectorType_& g) override
	{
		g.setZero();
		for(int64_t i = 0; i < objective_functions_.size(); i++)
		{
			auto& objective_function = objective_functions_.at(i);
			auto w = objective_function->GetWeight();
			objective_function->AddGradient<VectorType_>(g, w);
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

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunctionType_>> objective_functions_;
	bool parallel_update_;
	bool enforce_children_psd_;
};

#endif