#pragma once

#include <libs/optimization_lib/include/solvers/solver.h>

class GradientDescent : public Solver
{
public:
	GradientDescent() {}
	virtual double step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
};