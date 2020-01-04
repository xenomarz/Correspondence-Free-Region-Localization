#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <libs/optimization_lib/include/objective_functions/ObjectiveFunction.h>
#include <libs/optimization_lib/include/objective_functions/LagrangianLscmStArea.h>
#include <ext/worhp/worhp/worhp.h>

class worhpSolver
{
public:
	worhpSolver();
	~worhpSolver();
	Eigen::VectorXd run(
		const Eigen::MatrixXd& V,
		const Eigen::MatrixX3i& F,
		const Eigen::VectorXd& initialPoint
	);
	int get_data(Eigen::VectorXd& data);
	bool IsDataReady;
private:
	Eigen::VectorXd lastX;
	LagrangianLscmStArea* functionF;
	LagrangianLscmStArea* functionG;
	void update_data(OptVar* opt);
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