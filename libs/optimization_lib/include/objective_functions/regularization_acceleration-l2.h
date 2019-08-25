#pragma once

#include "Utils.h"
#include <functional>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include "ObjectiveFunction.h"

using namespace Eigen;
typedef Triplet<double> T;
typedef SparseMatrix<double> SpMat;
typedef Matrix<double, 6, 6> Matrix6d;
typedef Matrix<double, 6, 1> Vector6d;

class RegularizationAccelerationL2 : public ObjectiveFunction
{

public:

	/**************************************************************************************************************************/
	//INITIALIZATION 
	RegularizationAccelerationL2();

	virtual void init();
	virtual void updateX(const VectorXd& X);
	virtual double value();
	virtual void gradient(VectorXd& g);
	virtual void hessian();

	//loop implementation
	void prepare_hessian();
	/****************************************************************************************************************************/
    MatrixXd AX,AY;  // holds the accelerations
    int timeSteps=0;
    int numV=0;
};