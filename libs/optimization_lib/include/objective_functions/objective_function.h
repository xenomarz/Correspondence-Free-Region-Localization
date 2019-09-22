#pragma once

#include <libs/optimization_lib/include/utils.h>

class ObjectiveFunction
{
protected:
	/*virtual void prepare_dJdX() = 0;*/
	virtual void init_hessian() = 0;

public:
	ObjectiveFunction() {}
	virtual ~ObjectiveFunction(){}
	virtual void init() = 0;
	virtual void updateX(const VectorXd& X) = 0;
	virtual double value(const bool update = true) = 0;
	virtual void gradient(VectorXd& g) = 0;
	virtual void hessian() = 0;
	
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
	double energy_value = 0;
	double gradient_norm = 0;
	string name = "Objective function";
};

