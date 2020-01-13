#pragma once
#include "libs/optimization_lib/include/objective_functions/TriangleMeshObjectiveFunction.h"

typedef Eigen::Triplet<double> T;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Matrix<double, 6, 6> Matrix6d;
typedef Eigen::Matrix<double, 6, 1> Vector6d;

class SymmetricDirichletCompositeMajorization : public TriangleMeshObjectiveFunction
{
private:
	// cones alpha and beta
	Eigen::MatrixX2d alpha;
	Eigen::MatrixX2d beta;

	//singular values
	Eigen::MatrixX2d s; //Singular values s[0]>s[1]
	Eigen::MatrixX4d v; //Singular vectors 
	Eigen::MatrixX4d u; //Singular vectors 
	Eigen::MatrixXd Dsd[2]; //singular values dense derivatives s[0]>s[1]

	Eigen::SparseMatrix<double> a1, a1t, a2, a2t, b1, b1t, b2, b2t;     //constant matrices for cones calculation
	Eigen::MatrixXd a1d, a2d, b1d, b2d;					//dense constant matrices for cones calculation

	//SVD methods
	bool update_variables(const Eigen::VectorXd& X);
	void UpdateSSVDFunction();
	void ComputeDenseSSVDDerivatives();

	inline Matrix6d ComputeFaceConeHessian(const Vector6d& A1, const Vector6d& A2, double a1x, double a2x);
	inline Matrix6d ComputeConvexConcaveFaceHessian(const Vector6d& a1, const Vector6d& a2, const Vector6d& b1, const Vector6d& b2, double aY, double bY, double cY, double dY, const Vector6d& dSi, const Vector6d& dsi, double gradfS, double gradfs, double HS, double Hs);
public:
	SymmetricDirichletCompositeMajorization();
	virtual void init() override;
	virtual double value(const bool update) override;
	virtual void gradient(Eigen::VectorXd& g, const bool update) override;
    virtual void hessian() override;	
};