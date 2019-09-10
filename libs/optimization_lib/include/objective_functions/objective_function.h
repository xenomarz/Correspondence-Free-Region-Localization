#pragma once

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SparseCore>
#include <functional>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <memory>
#include <libs/optimization_lib/include/utils.h>
#include <limits>
#include <igl/doublearea.h>
#include <chrono>

using namespace Eigen;
using namespace std;

typedef Triplet<double> T;
typedef SparseMatrix<double> SpMat;
typedef Matrix<double, 6, 6> Matrix6d;
typedef Matrix<double, 6, 1> Vector6d;

class ObjectiveFunction
{
public:
	ObjectiveFunction() {}
	virtual ~ObjectiveFunction(){}
	virtual void init() = 0;
	virtual void updateX(const VectorXd& X) = 0;
	virtual double value() = 0;
	virtual void gradient(VectorXd& g) = 0;
	virtual void hessian() = 0;
	virtual void prepare_hessian() = 0;
	
	//Finite Differences check point
    void FDGradient(const VectorXd& X,VectorXd& g);
    void FDHessian(const VectorXd& X);
    bool checkGradient(const VectorXd& X);
    bool checkHessian(const VectorXd& X);
	
	// Hessian sparse reprensentation
	vector<int> II, JJ;
	vector<double> SS;

	//weight for each objective function
	float w;
	float Shift_eigen_values = 1e-6;
	VectorXd Efi;     //Efi=sum(Ef_dist.^2,2), for data->Efi history
	double energy_value = 0;
	double gradient_norm = 0;
	char* name = "Objective function";
};

