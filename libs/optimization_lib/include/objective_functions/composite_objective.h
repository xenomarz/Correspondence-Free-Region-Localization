#pragma once
#ifndef OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H
#define OPTIMIZATION_LIB_COMPOSITE_OBJECTIVE_H

// STL includes
#include <memory>
#include <vector>

// Optimization lib includes
#include "./objective_function.h"

template<Eigen::StorageOptions StorageOrder>
class CompositeObjective : public ObjectiveFunction<StorageOrder>
{
public:
	/**
	 * Constructors and destructor
	 */
	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, bool explicitly_zero_diagonal = false) :
		ObjectiveFunction(objective_function_data_provider, "Composite Objective"),
		explicitly_zero_diagonal_(explicitly_zero_diagonal)
	{
		this->Initialize();
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name, bool explicitly_zero_diagonal = false) :
		ObjectiveFunction(objective_function_data_provider, name),
		explicitly_zero_diagonal_(explicitly_zero_diagonal)
	{
		this->Initialize();
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>>& objective_functions, const std::string& name, bool explicitly_zero_diagonal = false) :
		CompositeObjective(objective_function_data_provider, name, explicitly_zero_diagonal)
	{
		AddObjectiveFunctions(objective_functions);
	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction<StorageOrder>> objective_function, const std::string& name, bool explicitly_zero_diagonal = false) :
		CompositeObjective(objective_function_data_provider, std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>>{ objective_function }, name, explicitly_zero_diagonal)
	{

	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>>& objective_functions, bool explicitly_zero_diagonal = false) :
		CompositeObjective(objective_function_data_provider, objective_functions, "Composite Objective", explicitly_zero_diagonal)
	{

	}

	CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction<StorageOrder>> objective_function, bool explicitly_zero_diagonal = false) :
		CompositeObjective(objective_function_data_provider, objective_function, "Composite Objective", explicitly_zero_diagonal)
	{

	}

	~CompositeObjective()
	{

	}

	/**
	 * Public Methods
	 */
	void AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction<StorageOrder>>& objective_function)
	{
		std::lock_guard<std::mutex> lock(m_);
		objective_functions_.push_back(objective_function);
	}

	void AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>>& objective_functions)
	{
		std::lock_guard<std::mutex> lock(m_);
		for (auto& objective_function : objective_functions)
		{
			objective_functions_.push_back(objective_function);
		}
	}

	void RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunction<StorageOrder>>& objective_function)
	{
		std::lock_guard<std::mutex> lock(m_);
		objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
	}

	void RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>>& objective_functions)
	{
		std::lock_guard<std::mutex> lock(m_);
		for (const auto& objective_function : objective_functions)
		{
			objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
		}
	}

	const std::uint32_t GetObjectiveFunctionsCount() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return objective_functions_.size();
	}

	const std::shared_ptr<ObjectiveFunction<StorageOrder>> GetObjectiveFunction(std::uint32_t index) const
	{
		std::lock_guard<std::mutex> lock(m_);
		if (index < objective_functions_.size())
		{
			return objective_functions_.at(index);
		}

		return nullptr;
	}

	const std::shared_ptr<ObjectiveFunction<StorageOrder>> GetObjectiveFunction(const std::string& name) const
	{
		std::lock_guard<std::mutex> lock(m_);
		for(auto& objective_function : objective_functions_)
		{
			if(!objective_function->GetName().compare(name))
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
	void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) override
	{
		f = 0;
		for (const const auto& objective_function : objective_functions_)
		{
			auto w = objective_function->GetWeight();
			if (w != 0)
			{
				f += w * objective_function->GetValue();
			}
		}
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		g.setZero();
		for (const auto& objective_function : objective_functions_)
		{
			auto w = objective_function->GetWeight();
			if (w != 0)
			{
				g += w * objective_function->GetGradient();
			}
		}
	}

	void CalculateHessian(Eigen::SparseMatrix<double, StorageOrder>& H) override
	{
		H.setZero();

		if(explicitly_zero_diagonal_)
		{
			for(int64_t i = 0; i < this->variables_count_; i++)
			{
				H.coeffRef(i, i) = 0;
			}
		}

		for (const auto& objective_function : objective_functions_)
		{
			auto w = objective_function->GetWeight();
			if (w != 0)
			{
				H += w * objective_function->GetHessian();
			}
		}
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
		for (const auto& objective_function : objective_functions_)
		{
			objective_function->Update(x);
		}
	}
	
	/**
	 * Fields
	 */
	std::vector<std::shared_ptr<ObjectiveFunction<StorageOrder>>> objective_functions_;
	bool explicitly_zero_diagonal_;
};

#endif