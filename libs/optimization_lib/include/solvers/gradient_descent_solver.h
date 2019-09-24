#pragma once

#include <libs/optimization_lib/include/solvers/solver.h>

class gradient_descent_solver : public solver
{
public:
	gradient_descent_solver() {}
	virtual double step() override;
	virtual bool test_progress() override;
	virtual void internal_init() override;
};