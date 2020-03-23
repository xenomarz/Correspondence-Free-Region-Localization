#pragma once
#ifndef OPTIMIZATION_LIB_REGION_LOCALIZATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_REGION_LOCALIZATION_OBJECTIVE_H

// STL includes
#include <vector>

// Optimization lib includes
#include "../data_providers/plain_data_provider.h"
#include "./dense_objective_function.h"

// Spectra
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/DenseSymMatProd.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/SymGEigsSolver.h>
#include <Spectra/MatOp/SparseCholesky.h>

template<Eigen::StorageOptions StorageOrder_>
class RegionLocalizationObjective : public DenseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Delta = DenseObjectiveFunction<StorageOrder_>::Properties::Count_,
		ValuePerEdge
	};
	
	/**
	 * Constructors and destructor
	 */
	RegionLocalizationObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const Eigen::VectorXd& mu, const std::shared_ptr<EmptyDataProvider>& empty_data_provider) :
		DenseObjectiveFunction(mesh_data_provider, empty_data_provider, "Region Localization", 0, false),
		mu_(mu),
		tau_(10 * mu.coeff(0))
	{
		half_tau_ = tau_ / 2;
		this->Initialize();
	}

	virtual ~RegionLocalizationObjective()
	{

	}

	/**
	 * Setters
	 */

	/**
	 * Getters
	 */
	double GetTau() const
	{
		return tau_;
	}

	const Eigen::VectorXd& GetSigma() const
	{
		return sigma_;
	}

	const Eigen::VectorXd& GetMu() const
	{
		return mu_;
	}

	const Eigen::VectorXd& GetLambda() const
	{
		return lambda_;
	}

private:

	/**
	 * Overrides
	 */
	void CalculateValue(double& f) override
	{
		f = 0;
		for(int64_t i = 0; i < lambda_.rows(); i++)
		{
			double diff = lambda_.coeff(i) - mu_.coeff(i);
			double factor = 1 / (mu_.coeff(i) * mu_.coeff(i));
			f += factor * (diff * diff);
		}
	}

	void CalculateValuePerVertex(Eigen::VectorXd& f_per_vertex) override
	{

	}

	void CalculateValuePerEdge(Eigen::VectorXd& domain_value_per_edge, Eigen::VectorXd& image_value_per_edge) override
	{

	}
	
	void CalculateGradient(Eigen::VectorXd& g) override
	{
		Eigen::MatrixXd phi_squared = Eigen::MatrixXd::Zero(phi_.rows(), phi_.cols());

		//#pragma omp parallel for
		//for(int64_t row = 0; row < phi_.rows(); row++)
		//{
		//	for (int64_t col = 0; col < phi_.cols(); col++)
		//	{
		//		phi_squared.coeffRef(row, col) = phi_.coeffRef(row, col) * phi_.coeffRef(row, col);
		//	}
		//}

		phi_squared = phi_.cwiseProduct(phi_);
		
		//Eigen::VectorXd v_tanh_squared = Eigen::VectorXd::Zero(v_.rows());
		//
		//#pragma omp parallel for
		//for (int64_t row = 0; row < v_.rows(); row++)
		//{
		//	v_tanh_squared.coeffRef(row) = v_tanh_.coeffRef(row) * v_tanh_.coeffRef(row);
		//}
		
		Eigen::VectorXd mu_squared = mu_.cwiseProduct(mu_);
		Eigen::VectorXd diff = (lambda_ - mu_).cwiseQuotient(mu_squared);
		g = 2 * phi_squared * diff;

		//#pragma omp parallel for
		//for (int64_t row = 0; row < v_.rows(); row++)
		//{
		//	g.coeffRef(row) = half_tau_ * g.coeffRef(row) * (1 - v_tanh_squared.coeffRef(row));
		//}
	}
	
	void PreUpdate(const Eigen::VectorXd& v) override
	{
		v_ = v;
		v_tanh_ = v_.array().tanh();
		sigma_ = half_tau_ * (v_tanh_ + Eigen::VectorXd::Ones(v_.rows()));
		Eigen::SparseMatrix<double> diag_v = v_.asDiagonal().toDenseMatrix().sparseView();
		Eigen::SparseMatrix<double> W = this->GetMeshDataProvider()->GetLaplacian();
		Eigen::SparseMatrix<double> A = this->GetMeshDataProvider()->GetMassMatrix();
		Eigen::SparseMatrix<double> lhs = W + A * diag_v;
		Eigen::SparseMatrix<double> rhs = A;
		Spectra::SparseSymMatProd<double> lhs_op(lhs);
		Spectra::SparseCholesky<double> rhs_op(rhs);
		Spectra::SymGEigsSolver<double, Spectra::SMALLEST_MAGN, Spectra::SparseSymMatProd<double>, Spectra::SparseCholesky<double>, Spectra::GEIGS_CHOLESKY> geigs(&lhs_op, &rhs_op, RDS_NEV, RDS_NCV);
		
		geigs.init();
		int nconv = geigs.compute();
		if (geigs.info() == Spectra::SUCCESSFUL)
		{
			lambda_ = geigs.eigenvalues();
			lambda_.conservativeResize(lambda_.rows() - 1);
			phi_ = geigs.eigenvectors();
			phi_.conservativeResize(phi_.rows(), phi_.cols() - 1);
		}
		else
		{
			bla = geigs.info();
		}

		//lambda_ = geigs.eigenvalues();
		//lambda_.conservativeResize(lambda_.rows() - 1);
		//phi_ = geigs.eigenvectors();
		//phi_.conservativeResize(phi_.rows(), phi_.cols() - 1);
	}
	
	void PreInitialize() override
	{

	}
	
	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{

	}
	
	void CalculateRawTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{

	}
	
	/**
	 * Fields
	 */
	Eigen::VectorXd v_tanh_;
	Eigen::VectorXd v_;
	Eigen::VectorXd mu_;
	Eigen::VectorXd lambda_;
	Eigen::MatrixXd phi_;
	Eigen::VectorXd sigma_;
	double tau_;
	double half_tau_;
	int bla;
};

#endif