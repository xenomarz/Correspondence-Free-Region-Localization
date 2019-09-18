#pragma once
#ifndef OPTIMIZATION_LIB_NEWTON_METHOD
#define OPTIMIZATION_LIB_NEWTON_METHOD

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
template <class Derived>
class NewtonMethod : public IterativeMethod
{
public:
	NewtonMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
		IterativeMethod(objective_function, x0)
	{

	}

	virtual ~NewtonMethod()
	{

	}

private:
	void ComputeDescentDirection(Eigen::VectorXd& p)
	{
		auto objective_function = GetObjectiveFunction();
		solver.Solve(objective_function->GetHessian(), -objective_function->CalculateGradient(), p);
	}

	std::enable_if_t<std::is_base_of<Solver, Derived>::value, Derived> solver;
};

#endif