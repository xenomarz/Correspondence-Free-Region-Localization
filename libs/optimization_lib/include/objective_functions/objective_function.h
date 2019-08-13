#pragma once
#include <Eigen/Core>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <Eigen/SparseCore>

using namespace Eigen;

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
	float w;
    std::string name="Objective function";
	
    void FDGradient(const VectorXd& X,VectorXd& g);
    void FDHessian(const VectorXd& X);
    bool checkGradient(const VectorXd& X);
    bool checkHessian(const VectorXd& X);
    // Hessian sparse reprensentation
	std::vector<int> II, JJ;
	std::vector<double> SS;

};

