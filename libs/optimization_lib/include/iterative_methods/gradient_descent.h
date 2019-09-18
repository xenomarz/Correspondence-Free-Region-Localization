#pragma once
#ifndef OPTIMIZATION_LIB_GRADIENT_DESCENT
#define OPTIMIZATION_LIB_GRADIENT_DESCENT

// STL includes
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./iterative_methods/iterative_method.h"
#include "../solvers/solver.h"

// https://en.wikipedia.org/wiki/Gradient_descent
class GradientDescent : public IterativeMethod
{
public:
	GradientDescent(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0);
	virtual ~GradientDescent();

private:
	void ComputeDescentDirection(Eigen::VectorXd& p);
};

#endif