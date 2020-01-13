#include "solvers/GradientDescentSolver.h"

double GradientDescentSolver::step()
{
	objective->updateX(X);
	if (IsConstrObjFunc)
		f = objective->AugmentedValue(true);
	else
		f = objective->value(true);

	objective->gradient(g,true);
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