#pragma once

#include <libs/optimization_lib/include/objective_functions/ObjectiveFunction.h>

class TriangleMeshObjectiveFunction: public ObjectiveFunction
{
protected:
	Eigen::MatrixXd grad;
	std::vector<Eigen::Matrix<double, 6, 6>> Hessian;
	std::vector<Eigen::Matrix<double, 4, 6>> dJ_dX;
	virtual bool update_variables(const Eigen::VectorXd& X);
	virtual void init_hessian();
public:
	TriangleMeshObjectiveFunction() {}
	virtual ~TriangleMeshObjectiveFunction(){}
	virtual void init() override;
	virtual void updateX(const Eigen::VectorXd& X) override;
	void AddElementToHessian(std::vector<int> ind);
};
