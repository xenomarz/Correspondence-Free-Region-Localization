#pragma once

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <objective_functions/objective_function.h>

class PenaltyPositionalConstraints : public ObjectiveFunction
{

public:

	/**************************************************************************************************************************/
	//INITIALIZATION 
	PenaltyPositionalConstraints();

	virtual void init();
	virtual void updateX(const VectorXd& X);
	virtual double value();
	virtual void gradient(Eigen::VectorXd& g);
	virtual void hessian();

	//loop implementation
	void prepare_hessian();

	std::vector<int> ConstrainedVerticesInd;
	Eigen::MatrixX2d ConstrainedVerticesPos;
	Eigen::MatrixX2d CurrConstrainedVerticesPos;
	int numV=0;
};