#include <solvers/GradientDescentSolver.h>

double GradientDescentSolver::step()
{
	objective->updateX(X);
	if (IsConstrObjFunc)
		f = objective->AugmentedValue();
	else
		f = objective->value();

	objective->gradient(g);
	p = -g;
	return f;
}

bool GradientDescentSolver::test_progress()
{
	return true;
}

void GradientDescentSolver::internal_init()
{
	objective->updateX(X);
	g.resize(X.size());
}