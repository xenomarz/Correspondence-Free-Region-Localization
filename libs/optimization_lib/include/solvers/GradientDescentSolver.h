#pragma once

#include "libs/optimization_lib/include/solvers/solver.h"

class GradientDescentSolver : public solver
{
public:
	GradientDescentSolver(const bool isConstrObjFunc, const int solverID) : solver(isConstrObjFunc,solverID) {}
	virtual void step() override;
	virtual void aug_step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
	virtual void internal_aug_init() override;
};