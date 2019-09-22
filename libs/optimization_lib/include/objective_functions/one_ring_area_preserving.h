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
	virtual void prepare_hessian() override;
	void prepare_dJdX();
	bool updateJ(const VectorXd& X);

public:
	OneRingAreaPreserving();

	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
};