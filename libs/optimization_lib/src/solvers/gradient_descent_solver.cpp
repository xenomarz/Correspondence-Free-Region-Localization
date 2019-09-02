#include <solvers/gradient_descent_solver.h>
#include <chrono>

GradientDescent::GradientDescent() {}

double GradientDescent::step()
{
	objective->updateX(X);
	f = objective->value();
	objective->gradient(g);
	p = -g;
	return f;
}

bool GradientDescent::test_progress()
{
	return true;
}

void GradientDescent::internal_init()
{
	objective->updateX(X);
	g.resize(X.size());
}