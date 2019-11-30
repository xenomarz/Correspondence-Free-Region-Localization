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
template <class Derived, Eigen::StorageOptions StorageOrder_>
class NewtonMethod : public IterativeMethod<StorageOrder_>
{
public:
	NewtonMethod(std::shared_ptr<ObjectiveFunction<StorageOrder_, Eigen::VectorXd>> objective_function, const Eigen::VectorXd& x0) :
		IterativeMethod(objective_function, x0)
	{
		// TODO: Call 'AnalyzePattern' after any objective function addition/removal
		solver_.AnalyzePattern(objective_function->GetHessian());
	}

	virtual ~NewtonMethod()
	{

	}

private:
	void ComputeDescentDirection(Eigen::VectorXd& p) override
	{
		auto objective_function = this->GetObjectiveFunction();
		solver_.Solve(objective_function->GetHessian(), -objective_function->GetGradient(), p);
	}

	std::enable_if_t<std::is_base_of<Solver<StorageOrder_>, Derived>::value, Derived> solver_;
};

#endif