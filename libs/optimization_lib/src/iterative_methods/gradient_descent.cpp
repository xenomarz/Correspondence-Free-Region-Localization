#include <iterative_methods/gradient_descent.h>

GradientDescent::GradientDescent(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
	IterativeMethod(objective_function, x0)
{

}

GradientDescent::~GradientDescent()
{

}

void GradientDescent::ComputeDescentDirection(Eigen::VectorXd& p)
{
	p = -GetObjectiveFunction()->GetGradient();
}