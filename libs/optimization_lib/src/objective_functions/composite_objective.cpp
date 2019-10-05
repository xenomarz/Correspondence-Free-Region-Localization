#include <objective_functions/composite_objective.h>

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions, const std::string& name) :
	ObjectiveFunction(objective_function_data_provider, name)
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

void CompositeObjective::AddObjectiveFunction(const std::shared_ptr<ObjectiveFunction> objective_function)
{
	std::lock_guard<std::mutex> lock(m_);
	objective_functions_.push_back(objective_function);
	Initialize();
}

void CompositeObjective::AddObjectiveFunctions(const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions)
{
	for (auto& objective_function : objective_functions)
	{
		objective_functions_.push_back(objective_function);
	}
	Initialize();
}

void CompositeObjective::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.clear();
	jj.clear();
	ss.clear();
	for (auto objective_function : objective_functions_)
	{
		ii.insert(ii.end(), objective_function->GetII().begin(), objective_function->GetII().end());
		jj.insert(jj.end(), objective_function->GetJJ().begin(), objective_function->GetJJ().end());
		ss.insert(ss.end(), objective_function->GetSS().begin(), objective_function->GetSS().end());
	}
}

void CompositeObjective::CalculateValue(const Eigen::VectorXd& x, double& f)
{
	f = 0;
	for (auto objective_function : objective_functions_)
	{
		auto w = objective_function->GetWeight();
		// TODO: uncomment line below, after switching to use vector of triplets instead of ii_, jj_ and ss_
		// if (objective_function->IsValid() && w != 0)
		if (w != 0)
		{
			f += w * objective_function->GetValue();
		}
	}
}

void CompositeObjective::CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g)
{
	g.setZero();
	for (auto objective_function : objective_functions_)
	{
		auto w = objective_function->GetWeight();
		// TODO: uncomment line below, after switching to use vector of triplets instead of ii_, jj_ and ss_
		// if (objective_function->IsValid() && w != 0)
		if (w != 0)
		{
			g += w * objective_function->GetGradient();
		}
	}
}

void CompositeObjective::CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss)
{
	ss.clear();
	for (auto objective_function : objective_functions_)
	{
		auto w = objective_function->GetWeight();
		// TODO: uncomment line below, after switching to use vector of triplets instead of ii_, jj_ and ss_
		// if (objective_function->IsValid() && w != 0)
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
	for (auto objective_function : objective_functions_)
	{
		objective_function->Initialize();
	}
}

void CompositeObjective::PreUpdate(const Eigen::VectorXd& x)
{
	// Postorder scan
	for (auto objective_function : objective_functions_)
	{
		objective_function->Update(x);
	}
}

bool CompositeObjective::IsValid()
{
	for (auto objective_function : objective_functions_)
	{
		if (objective_function->IsValid())
		{
			return true;
		}
	}

	return false;
}

const std::uint32_t CompositeObjective::GetObjectiveFunctionsCount() const
{
	return objective_functions_.size();
}

const std::shared_ptr<ObjectiveFunction> CompositeObjective::GetObjectiveFunction(std::uint32_t index) const
{
	return objective_functions_.at(index);
}