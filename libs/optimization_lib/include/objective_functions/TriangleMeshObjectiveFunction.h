#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class TriangleMeshObjectiveFunction: public ObjectiveFunction
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
	TriangleMeshObjectiveFunction() {}
	virtual ~TriangleMeshObjectiveFunction(){}
	
	void init_mesh(const MatrixXd& V, const MatrixX3i& F);

	VectorXd Efi;     
};
