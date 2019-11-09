#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <libs/optimization_lib/include/objective_functions/ObjectiveFunction.h>

class TriangleMeshObjectiveFunction: public ObjectiveFunction
{
public:
	TriangleMeshObjectiveFunction() {}
	virtual ~TriangleMeshObjectiveFunction(){}

	void init_mesh(const MatrixXd& V, const MatrixX3i& F);
	void AddElementToHessian(std::vector<int> ind);
};
