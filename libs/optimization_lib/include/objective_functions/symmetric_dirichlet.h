//#pragma once
//
////#include "EigenTypes.h"
////#include "AutocutsUtils.h"
////#include <functional>
//#include <Eigen/Core>
//#include <Eigen/Sparse>
//#include <vector>
//#include "objective_functions/objective_function.h"
//
//class SymmetricDirichlet : public ObjectiveFunction
//{
//public:
//	virtual void Update(const Eigen::VectorXd& x);
//
//protected:
//	virtual void PrepareHessian();
//
//private:
//	double bound=0;
//	Eigen::MatrixX3i F;
//	Eigen::MatrixX2d V;
//
//	int numV;
//	int numE;
//	int numS;
//	int numF;
//
//	//Jacobian of the parameterization per face
//	Eigen::VectorXd a;
//	Eigen::VectorXd b;
//	Eigen::VectorXd c;
//	Eigen::VectorXd d;
//	Eigen::VectorXd detJuv;		//(ad-bc)
//	Eigen::VectorXd invdetJuv;	//1/(ad-bc)
//	Eigen::SparseMatrix<double> DdetJuv_DUV; //jacobian of the function (detJuv) by UV
//
//	//singular values
//	Eigen::MatrixX2d s; //Singular values s[0]>s[1]
//	Eigen::MatrixX4d v; //Singular vectors 
//	Eigen::MatrixX4d u; //Singular vectors 
//	Eigen::MatrixXd Dsd[2]; //singular values dense derivatives s[0]>s[1]
//
//	//SVD methods
//	bool updateJ(const Eigen::MatrixX2d& X);
//	void UpdateSSVDFunction();
//	void ComputeDenseSSVDDerivatives();
//
//	//loop implementation
//	inline Eigen::Matrix<double, 6, 6> ComputeFaceConeHessian(const Eigen::Matrix<double,6,1> A1, const Eigen::Matrix<double, 6, 1>& A2, double a1x, double a2x);
//	inline Eigen::Matrix<double, 6, 6> ComputeConvexConcaveFaceHessian(
//		const Eigen::Matrix<double, 6, 1>& a1,
//		const Eigen::Matrix<double, 6, 1> & a2,
//		const Eigen::Matrix<double, 6, 1> & b1,
//		const Eigen::Matrix<double, 6, 1> & b2,
//		double aY,
//		double bY,
//		double cY,
//		double dY,
//		const Eigen::Matrix<double, 6, 1>& dSi,
//		const Eigen::Matrix<double, 6, 1>& dsi,
//		double gradfS,
//		double gradfs,
//		double HS,
//		double Hs);
//
//	// Energy parts
//	// distortion
//	Eigen::VectorXd Efi;     //Efi=sum(Ef_dist.^2,2), for data->Efi history
//
//	Eigen::MatrixXi Fuv;                             //F of cut mesh for u and v indices 6XnumF
//	Eigen::VectorXd Area;
//	Eigen::Matrix3Xd D1d, D2d;						//dense mesh derivative matrices
//
//	Eigen::SparseMatrix<double> a1, a1t, a2, a2t, b1, b1t, b2, b2t;     //constant matrices for cones calcualtion
//	Eigen::MatrixXd a1d, a2d, b1d, b2d;					//dense constant matrices for cones calcualtion
//
//	// per face hessians vector
//	std::vector<Eigen::Matrix<double,6,6>> Hi;
//
//	// pardiso variables
//	std::vector<int> II, JJ;
//	std::vector<double> SS;
//};