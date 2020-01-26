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

double GradientDescentSolver::aug_step()
{
	std::shared_ptr<TotalObjective> total = std::dynamic_pointer_cast<TotalObjective>(objective);
	assert(total != NULL);
	std::shared_ptr<LagrangianLscmStArea> aug_function = std::dynamic_pointer_cast<LagrangianLscmStArea>(total->objectiveList[0]);
	assert(aug_function != NULL);

	objective->updateX(X);
	f = objective->AugmentedValue(true);
	
	aug_function->AuglagrangGradWRTX(g, true);
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

void GradientDescentSolver::internal_aug_init()
{
	objective->updateX(X);
	g.resize(X.size()-F.rows());
}