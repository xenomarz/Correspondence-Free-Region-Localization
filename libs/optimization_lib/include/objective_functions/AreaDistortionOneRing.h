#pragma once
#include "libs/optimization_lib/include/objective_functions/ObjectiveFunction.h"

class AreaDistortionOneRing : public ObjectiveFunction
{	
private:
	// adjacency matrix (vertex to face)
	std::vector<std::vector<int>> VF, VFi;

	Eigen::VectorXd OneRingSum;
	std::vector<Eigen::RowVectorXd> grad;
	std::vector<Eigen::MatrixXd> dJ_dX;
	std::vector<Eigen::MatrixXd> Hessian;
	std::vector<std::vector<int>> OneRingVertices;
	std::vector<Eigen::MatrixXd> dE_dJ;

	std::vector<int> get_one_ring_vertices(const std::vector<int>& OneRingFaces);
	virtual void init_hessian() override;
	void init_dJdX();
	bool update_variables(const Eigen::VectorXd& X);

public:
	AreaDistortionOneRing();
	virtual void init() override;
	virtual void updateX(const Eigen::VectorXd& X) override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
	virtual void hessian() override;
};