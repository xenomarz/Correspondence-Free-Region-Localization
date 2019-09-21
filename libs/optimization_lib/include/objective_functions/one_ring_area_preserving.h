#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class OneRingAreaPreserving : public ObjectiveFunction
{	
private:
	// mesh vertices and faces
	MatrixX3i F;
	MatrixXd V;

	// Jacobian of the parameterization per face
	VectorXd a;
	VectorXd b;
	VectorXd c;
	VectorXd d;

	// adjacency matrix (vertex to face)
	vector<vector<int> > VF, VFi;

	// Jacobian determinant (ad-bc)
	VectorXd detJ;
	VectorXd OneRingSum;
	vector<RowVectorXd> grad;
	vector<MatrixXd> dJ_dX;
	vector<MatrixXd> Hessian;

	//Energy parts - distortion
	VectorXd Area;
	Matrix3Xd D1d, D2d;		//dense mesh derivative matrices

	vector<int> get_one_ring_vertices(const vector<int>& OneRingFaces);

public:
	OneRingAreaPreserving();

	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;
	bool updateJ(const VectorXd& X);
	void setVF(MatrixXd& V, MatrixX3i& F);
};