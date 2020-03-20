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
		mu_(mu)
	{
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
			f += factor * diff;
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
		Eigen::MatrixXd phi_squared = phi_.cwiseProduct(phi_);
		Eigen::VectorXd mu_squared = mu_.cwiseProduct(mu_);
		Eigen::MatrixXd diff = (lambda_ - mu_).cwiseQuotient(mu_squared);
		g = 2 * phi_squared * diff;
	}
	
	void PreUpdate(const Eigen::VectorXd& v) override
	{
		Eigen::SparseMatrix<double> diag_v = v.asDiagonal().toDenseMatrix().sparseView();
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
			phi_ = geigs.eigenvectors();
		}
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
	 * Private methods
	 */
	Eigen::VectorXd mu_;
	Eigen::VectorXd lambda_;
	Eigen::MatrixXd phi_;
	
	/**
	 * Fields
	 */
	std::shared_ptr<MeshDataProvider> mesh_data_provider_partial_;
};

#endif