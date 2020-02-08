#pragma once
#include "libs/optimization_lib/include/utils.h"

class ObjectiveFunction
{
//protected:
public:
	// mesh vertices and faces
	Eigen::MatrixX3i F;
	Eigen::MatrixXd V;

	// Jacobian of the parameterization per face
	Eigen::VectorXd a;
	Eigen::VectorXd b;
	Eigen::VectorXd c;
	Eigen::VectorXd d;
	Eigen::VectorXd detJ;
	Eigen::VectorXd Area;

	//dense mesh derivative matrices
	Eigen::Matrix3Xd D1d, D2d;
public:
	ObjectiveFunction() {}
	virtual ~ObjectiveFunction(){}
	virtual void init() = 0;
	virtual void init_hessian() = 0;
	virtual void updateX(const Eigen::VectorXd& X) = 0;
	virtual double value(const bool update) = 0;
	virtual double AugmentedValue(const bool update) { return -1; }
	virtual void gradient(Eigen::VectorXd& g, const bool update) = 0;
	virtual void hessian() = 0;
	
	void init_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixX3i& F);

	//Finite Differences check point
	Eigen::VectorXd FDGradient(const Eigen::VectorXd& X, const int type);
    void FDHessian(const Eigen::VectorXd& X);
    void checkGradient(const Eigen::VectorXd& X, const int type,const std::string str);
    bool checkHessian(const Eigen::VectorXd& X);
	bool checkAugGradient(const Eigen::VectorXd& X);
	bool checkAugHessian(const Eigen::VectorXd& X);
	void FDAugGradient(const Eigen::VectorXd& X, Eigen::VectorXd& g);
	void FDAugHessian(const Eigen::VectorXd& X);
	
	// Hessian sparse reprensentation
	std::vector<int> II, JJ;
	std::vector<double> SS;

	//weight for each objective function
	float w = 0;
	Eigen::VectorXd Efi;
	double energy_value = 0;
	double gradient_norm = 0;
	std::string name = "Objective function";
};

