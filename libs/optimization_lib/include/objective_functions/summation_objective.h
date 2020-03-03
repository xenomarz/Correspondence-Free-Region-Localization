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
	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const bool enforce_children_psd = false) :
		SummationObjective(mesh_data_provider, data_provider, "Summation Objective", enforce_children_psd)
	{

	}
	
	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name, const bool enforce_children_psd = false) :
		ObjectiveFunction(mesh_data_provider, data_provider, name),
		enforce_children_psd_(enforce_children_psd)
	{
		//this->Initialize();
	}

	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const std::string& name, const bool enforce_children_psd = false) :
		SummationObjective(mesh_data_provider, data_provider, name, enforce_children_psd)
	{
		AddObjectiveFunctions(objective_functions);
	}

	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const std::string& name, const bool enforce_children_psd = false) :
		SummationObjective(mesh_data_provider, data_provider, std::vector<std::shared_ptr<ObjectiveFunctionType_>>{ objective_function }, name, enforce_children_psd)
	{

	}

	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions, const bool enforce_children_psd = false) :
		SummationObjective(mesh_data_provider, data_provider, "Summation Objective", enforce_children_psd)
	{
		AddObjectiveFunctions(objective_functions);
	}

	SummationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::shared_ptr<ObjectiveFunctionType_> objective_function, const bool enforce_children_psd = false) :
		SummationObjective(mesh_data_provider, data_provider, std::vector<std::shared_ptr<ObjectiveFunctionType_>>{ objective_function }, "Summation Objective", enforce_children_psd)
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
		objective_functions_.push_back(objective_function);
		this->dependencies_.push_back(objective_function);
	}

	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		for (auto& objective_function : objective_functions)
		{
			objective_functions_.push_back(objective_function);
			this->dependencies_.push_back(objective_function);
		}
	}

	void RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunctionType_>& objective_function)
	{
		objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
		this->dependencies_.erase(std::remove(this->dependencies_.begin(), this->dependencies_.end(), objective_function), this->dependencies_.end());
	}

	void RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunctionType_>>& objective_functions)
	{
		for (const auto& objective_function : objective_functions)
		{
			objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
			this->dependencies_.erase(std::remove(this->dependencies_.begin(), this->dependencies_.end(), objective_function), this->dependencies_.end());
		}
	}

	std::size_t GetObjectiveFunctionsCount() const
	{
		return objective_functions_.size();
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(std::uint32_t index) const
	{
		if (index < objective_functions_.size())
		{
			return objective_functions_.at(index);
		}

		return nullptr;
	}

	std::shared_ptr<ObjectiveFunctionType_> GetObjectiveFunction(const std::string& name) const
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

protected:
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

	void CalculateValuePerEdge(Eigen::VectorXd& domain_value_per_edge, Eigen::VectorXd& image_value_per_edge) override
	{
		// TODO: Add implementation
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
		// Empty implementation
	}

	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunctionType_>> objective_functions_;
	bool parallel_update_;
	bool enforce_children_psd_;
};

#endif