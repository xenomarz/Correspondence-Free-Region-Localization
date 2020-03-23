#pragma once
#ifndef OPTIMIZATION_LIB_PROJECTED_GRADIENT_DESCENT_H
#define OPTIMIZATION_LIB_PROJECTED_GRADIENT_DESCENT_H

// STL includes
#include <memory>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./iterative_method.h"

// https://en.wikipedia.org/wiki/Gradient_descent
template <Eigen::StorageOptions StorageOrder_>
class ProjectedGradientDescent : public IterativeMethod<StorageOrder_>
{
public:
	ProjectedGradientDescent(std::shared_ptr<ObjectiveFunction<StorageOrder_, Eigen::VectorXd>> objective_function, const Eigen::VectorXd& x0) :
		IterativeMethod(objective_function, x0)
	{

	}

	virtual ~ProjectedGradientDescent()
	{

	}

private:
	void ComputeDescentDirection(Eigen::VectorXd& p) override
	{
		//Eigen::VectorXd g = this->GetObjectiveFunction()->GetGradient();
		//Eigen::VectorXd g_max = g.cwiseMax(Eigen::VectorXd::Zero(p.rows()));
		//p = -g_max;
		p = -this->GetObjectiveFunction()->GetGradient();
	}
};

#endif