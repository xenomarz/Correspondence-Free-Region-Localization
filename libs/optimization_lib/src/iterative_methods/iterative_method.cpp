#include <iterative_methods/iterative_method.h>

IterativeMethod::IterativeMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
	objective_function_(objective_function),
	x_(x0),
	is_running_(false)
{

}

IterativeMethod::~IterativeMethod()
{
	is_running_ = false;
	thread_.join();
}

void IterativeMethod::Run()
{
	if (!is_running_)
	{
		thread_ = std::thread([&] () {
			while (is_running_)
			{
				
			}
		});
	}
}

void IterativeMethod::Stop()
{

}

void IterativeMethod::GetApproximation(Eigen::VectorXd& x)
{

}

void IterativeMethod::EnableFlipAvoidingLineSearch(Eigen::MatrixX3i& f)
{

}

void IterativeMethod::DisableFlipAvoidingLineSearch(Eigen::MatrixX3i& f)
{

}