#pragma once
#ifndef OPTIMIZATION_LIB_SEPARATION_H
#define OPTIMIZATION_LIB_SEPARATION_H

// STL includes
#include <vector>
#include <map>
#include <list>

// Optimization lib includes
#include "./objective_function.h"

class Separation : public ObjectiveFunction
{
public:

	/**
	 * Constructors and destructor
	 */
	Separation(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);
	virtual ~Separation();

	/**
	 * Public methods
	 */
	void SetDelta(const double delta);
	double GetDelta() const;

private:

	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) override;
	void CalculateValue(const Eigen::VectorXd& x, double& f, Eigen::VectorXd& f_per_vertex) override;
	void CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g) override;
	void CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss) override;
	void PreUpdate(const Eigen::VectorXd& x) override;
	void PreInitialize() override;

	/**
	 * Methods
	 */
	void FindSingleHessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h);

	/**
	 * Fields
	 */
	double delta_ = 1.0;

	Eigen::MatrixX2d X;

	Eigen::SparseMatrix<double> Esep;
	Eigen::SparseMatrix<double> Esept;
	Eigen::MatrixX2d EsepP;

	Eigen::VectorXd f_per_pair;
	Eigen::VectorXd f_sep_per_pair;

	// weighting indicated by the coloring of the mesh
	// alphas gathered by summing up the factors
	// for each corner force
	Eigen::VectorXd connect_alphas;

	// same vars for disconnect
	Eigen::VectorXd disconnect_alphas;

	Eigen::VectorXd edge_lenghts_per_pair;
	Eigen::VectorXd no_seam_constraints_per_pair;

	Eigen::VectorXd EsepP_squared_rowwise_sum;
	Eigen::VectorXd EsepP_squared_rowwise_sum_plus_delta;
};

#endif