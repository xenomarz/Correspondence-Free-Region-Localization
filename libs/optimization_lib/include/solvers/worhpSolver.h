#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <ext/worhp/worhp/worhp.h>

class worhpSolver
{
public:
	int run();
private:
	// Objective function
	void UserF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Function of constraints
	void UserG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Gradient of objective function
	void UserDF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Jacobian of constraints
	void UserDG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Hessian of Lagrangian
	void UserHM(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
};