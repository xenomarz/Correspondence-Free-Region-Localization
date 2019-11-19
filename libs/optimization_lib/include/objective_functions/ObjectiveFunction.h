#pragma once
#include <libs/optimization_lib/include/utils.h>

class ObjectiveFunction
{
protected:
	// mesh vertices and faces
	MatrixX3i F;
	MatrixXd V;

	// Jacobian of the parameterization per face
	VectorXd a;
	VectorXd b;
	VectorXd c;
	VectorXd d;
	VectorXd detJ;
	VectorXd Area;

	//dense mesh derivative matrices
	Matrix3Xd D1d, D2d;		
public:
	ObjectiveFunction() {}
	virtual ~ObjectiveFunction(){}
	virtual void init() = 0;
	virtual void init_hessian() = 0;
	virtual void updateX(const VectorXd& X) = 0;
	virtual double value(const bool update = true) = 0;
	virtual double AugmentedValue() { return -1; }
	virtual void gradient(VectorXd& g) = 0;
	virtual void hessian() = 0;
	
	void init_mesh(const MatrixXd& V, const MatrixX3i& F);

	//Finite Differences check point
    void FDGradient(const VectorXd& X,VectorXd& g);
    void FDHessian(const VectorXd& X);
    bool checkGradient(const VectorXd& X);
    bool checkHessian(const VectorXd& X);
	
	// Hessian sparse reprensentation
	vector<int> II, JJ;
	vector<double> SS;

	//weight for each objective function
	float w = 0;
	float augmented_value_parameter = 1;
	VectorXd Efi;     
	double energy_value = 0;
	double objective_value = 0;
	double constraint_value = 0;

	double gradient_norm = 0;
	double objective_gradient_norm = 0;
	double constraint_gradient_norm = 0;
	string name = "Objective function";
};

