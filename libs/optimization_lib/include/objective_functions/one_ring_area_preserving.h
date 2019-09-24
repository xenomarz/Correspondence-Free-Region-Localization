#pragma once
#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class one_ring_area_preserving : public objective_function
{	
private:
	// adjacency matrix (vertex to face)
	vector<vector<int> > VF, VFi;

	VectorXd OneRingSum;
	vector<RowVectorXd> grad;
	vector<MatrixXd> dJ_dX;
	vector<MatrixXd> Hessian;
	vector<vector<int>> OneRingVertices;

	vector<int> get_one_ring_vertices(const vector<int>& OneRingFaces);
	virtual void init_hessian() override;
	void init_dJdX();
	bool update_variables(const VectorXd& X);

public:
	one_ring_area_preserving();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};