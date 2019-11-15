#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <libs/optimization_lib/include/objective_functions/ObjectiveFunction.h>

class TriangleMeshObjectiveFunction: public ObjectiveFunction
{
protected:
	MatrixXd grad;
	vector<Matrix<double, 6, 6>> Hessian;
	vector<Matrix<double, 4, 6>> dJ_dX;
	virtual bool update_variables(const VectorXd& X);
	virtual void init_hessian();
public:
	TriangleMeshObjectiveFunction() {}
	virtual ~TriangleMeshObjectiveFunction(){}

	void AddElementToHessian(std::vector<int> ind);
};
