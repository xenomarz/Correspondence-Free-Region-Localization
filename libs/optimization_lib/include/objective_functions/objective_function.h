#pragma once
#include <Eigen/Core>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <Eigen/SparseCore>

using namespace Eigen;
using namespace std;

class ObjectiveFunction
{
public:
	ObjectiveFunction();
	virtual ~ObjectiveFunction();
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
	string name = "Objective function";
};

