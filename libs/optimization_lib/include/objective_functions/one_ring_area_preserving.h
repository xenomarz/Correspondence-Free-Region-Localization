#pragma once
#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class OneRingAreaPreserving : public ObjectiveFunction
{	
private:
	// adjacency matrix (vertex to face)
	vector<vector<int> > VF, VFi;

	// Jacobian determinant (ad-bc)
	VectorXd OneRingSum;
	vector<RowVectorXd> grad;
	vector<MatrixXd> dJ_dX;
	vector<MatrixXd> Hessian;

	vector<int> get_one_ring_vertices(const vector<int>& OneRingFaces);
	virtual void init_hessian() override;
	void init_dJdX();
	bool update_variables(const VectorXd& X);

public:
	OneRingAreaPreserving();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};