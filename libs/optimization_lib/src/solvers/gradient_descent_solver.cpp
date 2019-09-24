#include <solvers/gradient_descent_solver.h>

double gradient_descent_solver::step()
{
	objective->updateX(X);
	f = objective->value();
	objective->gradient(g);
	p = -g;
	return f;
}

bool gradient_descent_solver::test_progress()
{
	return true;
}

void gradient_descent_solver::internal_init()
{
	objective->updateX(X);
	g.resize(X.size());
}