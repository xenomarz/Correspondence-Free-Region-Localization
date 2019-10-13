#include <objective_functions/composite_objective.h>

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, bool explicitly_zero_diagonal) :
	ObjectiveFunction(objective_function_data_provider, "Composite Objective"),
	explicitly_zero_diagonal_(explicitly_zero_diagonal)
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name, bool explicitly_zero_diagonal) :
	ObjectiveFunction(objective_function_data_provider, name),
	explicitly_zero_diagonal_(explicitly_zero_diagonal)
{
	
}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions, const std::string& name, bool explicitly_zero_diagonal) :
	CompositeObjective(objective_function_data_provider, name, explicitly_zero_diagonal)
{
	AddObjectiveFunctions(objective_functions);
}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function, const std::string& name, bool explicitly_zero_diagonal) :
	CompositeObjective(objective_function_data_provider, std::vector<std::shared_ptr<ObjectiveFunction>>{ objective_function }, name, explicitly_zero_diagonal)
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::shared_ptr<ObjectiveFunction>>& objective_functions, bool explicitly_zero_diagonal) :
	CompositeObjective(objective_function_data_provider, objective_functions, "Composite Objective", explicitly_zero_diagonal)
{

}

CompositeObjective::CompositeObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::shared_ptr<ObjectiveFunction> objective_function, bool explicitly_zero_diagonal) :
	CompositeObjective(objective_function_data_provider, objective_function, "Composite Objective", explicitly_zero_diagonal)
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

	if (explicitly_zero_diagonal_)
	{
		std::vector<int> ii_diag;
		std::vector<int> jj_diag;
		std::vector<int> ss_diag;

		ii_diag.resize(variables_count_);
		jj_diag.resize(variables_count_);
		ss_diag.resize(variables_count_);

		for (int64_t i = 0; i < variables_count_; i++)
		{
			ii_diag[i] = i;
			jj_diag[i] = i;
			ss_diag[i] = 0;
		}

		ii.insert(ii.end(), ii_diag.begin(), ii_diag.end());
		jj.insert(jj.end(), jj_diag.begin(), jj_diag.end());
		ss.insert(ss.end(), ss_diag.begin(), ss_diag.end());
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

	if (explicitly_zero_diagonal_)
	{
		std::vector<int> ss_diag;
		ss_diag.resize(variables_count_);

		#pragma omp parallel for
		for (int64_t i = 0; i < variables_count_; i++)
		{
			ss_diag[i] = 0;
		}

		ss.insert(ss.end(), ss_diag.begin(), ss_diag.end());
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