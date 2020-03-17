#include "solvers/NewtonSolver.h"

void NewtonSolver::step()
{
	objective->updateX(X);
	currentEnergy = objective->value(true);
	objective->gradient(g,true);
	objective->hessian();
	eigen_solver->factorize(objective->II, objective->JJ, objective->SS);
	Eigen::VectorXd rhs = -g;
	p = eigen_solver->solve(rhs);
}

Eigen::SparseMatrix<double> NewtonSolver::get_Hessian()
{
	return eigen_solver->full_A;
}

double NewtonSolver::get_MSE()
{
	return eigen_solver->MSE;
}

bool NewtonSolver::test_progress()
{
	return true;
}

void NewtonSolver::internal_init()
{
	bool needs_init = eigen_solver == nullptr;
	if (needs_init) {
		eigen_solver = std::make_unique<EigenSparseSolver<std::vector<int>, std::vector<double>>>();
	}
	objective->updateX(X);
	g.resize(X.size());
	objective->hessian();
	if (needs_init) {
		eigen_solver->set_pattern(objective->II, objective->JJ, objective->SS);
		eigen_solver->analyze_pattern();
	}
}
