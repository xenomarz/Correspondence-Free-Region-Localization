#pragma once
#include <libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h>

class AreaDistortionOneRing : public TriangleMeshObjectiveFunction
{	
private:
	// adjacency matrix (vertex to face)
	vector<vector<int> > VF, VFi;

	VectorXd OneRingSum;
	vector<RowVectorXd> grad;
	vector<MatrixXd> dJ_dX;
	vector<MatrixXd> Hessian;
	vector<vector<int>> OneRingVertices;
	vector<MatrixXd> dE_dJ;

	vector<int> get_one_ring_vertices(const vector<int>& OneRingFaces);
	virtual void init_hessian() override;
	void init_dJdX();
	bool update_variables(const VectorXd& X);

public:
	AreaDistortionOneRing();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};