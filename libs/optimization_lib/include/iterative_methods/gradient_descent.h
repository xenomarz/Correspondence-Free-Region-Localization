#pragma once
#ifndef OPTIMIZATION_LIB_GRADIENT_DESCENT_H
#define OPTIMIZATION_LIB_GRADIENT_DESCENT_H

// STL includes
#include <memory>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./iterative_methods/iterative_method.h"

// https://en.wikipedia.org/wiki/Gradient_descent
template <Eigen::StorageOptions StorageOrder_>
class GradientDescent : public IterativeMethod<StorageOrder_>
{
public:
	GradientDescent::GradientDescent(std::shared_ptr<ObjectiveFunction<StorageOrder_>> objective_function, const Eigen::VectorXd& x0) :
		IterativeMethod(objective_function, x0)
	{

	}

	GradientDescent::~GradientDescent()
	{

	}

private:
	void ComputeDescentDirection(Eigen::VectorXd& p) override
	{
		p = -this->GetObjectiveFunction()->GetGradient();
	}
};

#endif