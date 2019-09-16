#pragma once
#ifndef OPTIMIZATION_LIB_SEPARATION
#define OPTIMIZATION_LIB_SEPARATION

// STL includes
#include <vector>
#include <map>
#include <list>

// Optimization lib includes
#include <objective_functions/objective_function.h>

class Separation : public ObjectiveFunction
{
private:
	Separation(const std::shared_ptr<MeshWrapper>& mesh_wrapper);
	virtual ~Separation();

	// Overrides
	void InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& x, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& x, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& x, std::vector<double>& ss);

	void FindSingleHessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h);

	Eigen::SparseMatrix<double> Esep;
	Eigen::SparseMatrix<double> Esept;
	Eigen::MatrixX2d EsepP;

	double delta = 1.0;

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