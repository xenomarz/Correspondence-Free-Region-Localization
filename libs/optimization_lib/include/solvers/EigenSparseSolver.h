#pragma once
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>

template <typename vectorTypeI, typename vectorTypeS>
class EigenSparseSolver
{
public:
	EigenSparseSolver();
	~EigenSparseSolver();
	void set_pattern(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS);
	void analyze_pattern();
	bool factorize(const vectorTypeI &II, const vectorTypeI &JJ, const vectorTypeS &SS);
	Eigen::VectorXd solve(Eigen::VectorXd &rhs);
    //Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;

	//Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
	/*Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
	Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;*/

    Eigen::SparseMatrix<double> A;
};

