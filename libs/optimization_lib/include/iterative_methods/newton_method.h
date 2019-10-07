#pragma once
#ifndef OPTIMIZATION_LIB_NEWTON_METHOD_H
#define OPTIMIZATION_LIB_NEWTON_METHOD_H

// STL includes
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./iterative_method.h"
#include "../solvers/solver.h"

// https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization
template <class Derived, Eigen::StorageOptions StorageOptions>
class NewtonMethod : public IterativeMethod
{
public:
	NewtonMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
		IterativeMethod(objective_function, x0)
	{
		switch (StorageOptions)
		{
		case Eigen::StorageOptions::ColMajor:
			solver_.AnalyzePattern(objective_function->GetHessianColMajor());
			break;
		case Eigen::StorageOptions::RowMajor:
			solver_.AnalyzePattern(objective_function->GetHessianRowMajor());
			break;
		}
	}

	virtual ~NewtonMethod()
	{

	}

private:
	void ComputeDescentDirection(Eigen::VectorXd& p)
	{
		auto objective_function = GetObjectiveFunction();
		switch (StorageOptions)
		{
		case Eigen::StorageOptions::ColMajor:
			solver_.Solve(objective_function->GetHessianColMajor(), -objective_function->GetGradient(), p);
			break;
		case Eigen::StorageOptions::RowMajor:
			solver_.Solve(objective_function->GetHessianRowMajor(), -objective_function->GetGradient(), p);
			break;
		}
	}

	std::enable_if_t<std::is_base_of<Solver, Derived>::value, Derived> solver_;
};

#endif