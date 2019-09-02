#include "objective_functions/objective_function.h"

ObjectiveFunction::ObjectiveFunction() :
	f(0)
{

}


ObjectiveFunction::~ObjectiveFunction()
{

}

double ObjectiveFunction::GetValue() const
{
	return f;
}

const Eigen::VectorXd& ObjectiveFunction::GetGradient() const
{
	return g;
}

const std::vector<int>& ObjectiveFunction::GetII() const
{
	return II;
}

const std::vector<int>& ObjectiveFunction::GetJJ() const
{
	return JJ;
}

const std::vector<double>& ObjectiveFunction::GetSS() const
{
	return SS;
}
