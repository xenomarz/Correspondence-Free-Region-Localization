#include <objective_functions/composite_objective.h>

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
	ObjectiveFunction(objective_function_data_provider, "Composite Objective")
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
	ObjectiveFunction(objective_function_data_provider, name)
{
	
}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions, const std::string& name) :
	CompositeObjective(objective_function_data_provider, name)
{
	AddObjectiveFunctions(objective_functions);
}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function, const std::string& name) :
	CompositeObjective(objective_function_data_provider, std::vector<std::shared_ptr<ObjectiveFunction>>{ objective_function }, name)
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions) :
	CompositeObjective(objective_function_data_provider, objective_functions, "Composite Objective")
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function) :
	CompositeObjective(objective_function_data_provider, objective_function, "Composite Objective")
{

}

CompositeObjective::~CompositeObjective()
{

}

void CompositeObjective::AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction>& objective_function)
{
	std::lock_guard<std::mutex> lock(m_);
	objective_functions_.push_back(objective_function);
	Initialize();
}

void CompositeObjective::AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions)
{
	std::lock_guard<std::mutex> lock(m_);
	for (auto& objective_function : objective_functions)
	{
		objective_functions_.push_back(objective_function);
	}
	Initialize();
}

void CompositeObjective::RemoveObjectiveFunction(const std::shared_ptr<ObjectiveFunction>& objective_function)
{
	std::lock_guard<std::mutex> lock(m_);
	objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
}

void CompositeObjective::RemoveObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions)
{
	std::lock_guard<std::mutex> lock(m_);
	for(const auto& objective_function : objective_functions)
	{
		objective_functions_.erase(std::remove(objective_functions_.begin(), objective_functions_.end(), objective_function), objective_functions_.end());
	}
}

void CompositeObjective::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.clear();
	jj.clear();
	ss.clear();
	for (const auto& objective_function : objective_functions_)
	{
		ii.insert(ii.end(), objective_function->GetII().begin(), objective_function->GetII().end());
		jj.insert(jj.end(), objective_function->GetJJ().begin(), objective_function->GetJJ().end());
		ss.insert(ss.end(), objective_function->GetSS().begin(), objective_function->GetSS().end());
	}
}

void CompositeObjective::CalculateValue(double& f, Eigen::VectorXd& f_per_vertex)
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

void CompositeObjective::CalculateGradient(Eigen::VectorXd& g)
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

void CompositeObjective::CalculateHessian(std::vector<double>& ss)
{
	ss.clear();
	for (const const auto& objective_function : objective_functions_)
	{
		const auto w = objective_function->GetWeight();
		if (w != 0)
		{
			auto current_ss = objective_function->GetSS();

			#pragma omp parallel for
			for (int i = 0; i < current_ss.size(); i++)
			{
				current_ss[i] = w * current_ss[i];
			}
			ss.insert(ss.end(), current_ss.begin(), current_ss.end());
		}
	}
}

void CompositeObjective::PreInitialize()
{
	// Preorder scan
	ObjectiveFunction::PreInitialize();
	for (const auto& objective_function : objective_functions_)
	{
		objective_function->Initialize();
	}
}

void CompositeObjective::PreUpdate(const Eigen::VectorXd& x)
{
	// Postorder scan
	for (const auto& objective_function : objective_functions_)
	{
		objective_function->Update(x);
	}
}

const std::uint32_t CompositeObjective::GetObjectiveFunctionsCount() const
{
	return objective_functions_.size();
}

const std::shared_ptr<ObjectiveFunction> CompositeObjective::GetObjectiveFunction(std::uint32_t index) const
{
	return objective_functions_.at(index);
}