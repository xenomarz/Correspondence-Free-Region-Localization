#pragma once

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include <objective_functions/objective_function.h>

class SymmetricDirichlet : public ObjectiveFunction
{
private:
	SymmetricDirichlet(const std::shared_ptr<MeshWrapper>& mesh_wrapper);
	virtual ~SymmetricDirichlet();

	// Overrides
	void InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& x, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& x, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& x, std::vector<double>& ss);

	double bound=0;
	Eigen::MatrixX3i F;
	Eigen::MatrixX2d V;

	int numV;
	int numE;
	int numS;
	int numF;

	/**
	 * Jacobian of the parameterization per face
	 */
	Eigen::VectorXd a;
	Eigen::VectorXd b;
	Eigen::VectorXd c;
	Eigen::VectorXd d;
	Eigen::VectorXd detJuv;						// (ad - bc)
	Eigen::VectorXd invdetJuv;					// 1 / (ad - bc)
	Eigen::SparseMatrix<double> DdetJuv_DUV;	// Jacobian of the function (detJuv) by UV

	/**
	 * Singular values
	 */
	Eigen::MatrixX2d s;							// Singular values s[0]>s[1]
	Eigen::MatrixX4d v;							// Singular vectors 
	Eigen::MatrixX4d u;							// Singular vectors 
	Eigen::MatrixXd Dsd[2];						// Singular values dense derivatives s[0]>s[1]

	/**
	 * SVD methods
	 */
	bool updateJ(const Eigen::MatrixX2d& X);
	void UpdateSSVDFunction();
	void ComputeDenseSSVDDerivatives();

	/**
	 * Loop implementation
	 */
	inline Eigen::Matrix<double, 6, 6> ComputeFaceConeHessian(
		const Eigen::Matrix<double, 6, 1>& A1,
		const Eigen::Matrix<double, 6, 1>& A2,
		double a1x,
		double a2x);

	inline Eigen::Matrix<double, 6, 6> ComputeConvexConcaveFaceHessian(
		const Eigen::Matrix<double, 6, 1>& a1,
		const Eigen::Matrix<double, 6, 1> & a2,
		const Eigen::Matrix<double, 6, 1> & b1,
		const Eigen::Matrix<double, 6, 1> & b2,
		double aY,
		double bY,
		double cY,
		double dY,
		const Eigen::Matrix<double, 6, 1>& dSi,
		const Eigen::Matrix<double, 6, 1>& dsi,
		double gradfS,
		double gradfs,
		double HS,
		double Hs);

	/**
	 * Energy
	 */

	// Efi = sum(Ef_dist.^2, 2), for data->Efi history
	Eigen::VectorXd Efi;

	// F of cut mesh for u and v indices 6XnumF
	Eigen::MatrixXi Fuv;
	Eigen::VectorXd Area;

	// Dense mesh derivative matrices
	Eigen::Matrix3Xd D1d, D2d;										

	// Constant matrices for cones calculation
	Eigen::SparseMatrix<double> a1, a1t, a2, a2t, b1, b1t, b2, b2t;	

	// Dense constant matrices for cones calculation
	Eigen::MatrixXd a1d, a2d, b1d, b2d;

	// Per face Hessians vector
	std::vector<Eigen::Matrix<double,6,6>> Hi;
};