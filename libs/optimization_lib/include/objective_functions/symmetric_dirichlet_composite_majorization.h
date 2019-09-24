#pragma once
#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class symmetric_dirichlet_composite_majorization : public objective_function
{
private:
	// cones alpha and beta
	MatrixX2d alpha;
	MatrixX2d beta;

	//singular values
	MatrixX2d s; //Singular values s[0]>s[1]
	MatrixX4d v; //Singular vectors 
	MatrixX4d u; //Singular vectors 
	MatrixXd Dsd[2]; //singular values dense derivatives s[0]>s[1]

	SparseMatrix<double> a1, a1t, a2, a2t, b1, b1t, b2, b2t;     //constant matrices for cones calcualtion
	MatrixXd a1d, a2d, b1d, b2d;					//dense constant matrices for cones calcualtion

	//SVD methods
	bool update_variables(const VectorXd& X);
	void UpdateSSVDFunction();
	void ComputeDenseSSVDDerivatives();

	inline Matrix6d ComputeFaceConeHessian(const Vector6d& A1, const Vector6d& A2, double a1x, double a2x);
	inline Matrix6d ComputeConvexConcaveFaceHessian(const Vector6d& a1, const Vector6d& a2, const Vector6d& b1, const Vector6d& b2, double aY, double bY, double cY, double dY, const Vector6d& dSi, const Vector6d& dsi, double gradfS, double gradfs, double HS, double Hs);

public:
	symmetric_dirichlet_composite_majorization();
	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value(const bool update = true) override;
	virtual void gradient(VectorXd& g) override;
    virtual void hessian() override;	
};