#pragma once

// STL includes
#include <vector>
#include <map>
#include <list>

// Optimization lib includes
#include "objective_functions/objective_function.h"

class Separation : public ObjectiveFunction
{
private:
	Separation(const std::shared_ptr<MeshWrapper>& mesh_wrapper);
	virtual ~Separation();

	// Overrides
	void InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& II, std::vector<int>& JJ, std::vector<double>& SS);
	void CalculateValue(const Eigen::MatrixX2d& X, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& SS);

	void FindSingleHessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h);

	Eigen::SparseMatrix<double> EVvar1, EVvar2, Esep, Esept, V2V, V2Vt;
	Eigen::SparseMatrix<double> C2C; //Corner to corner
	Eigen::MatrixX2d EsepP;

	double Lsep = 1.0;
	double delta = 1.0;

	Eigen::VectorXd f_per_pair, f_sep_per_pair;

	// force these uv vertices to be connected more closely, used for gradient
	std::vector<int> gradient_force_connects;

	// same for function value, to affect the correct index in f_per_row
	// since here its already sorted according to pairs
	std::vector<int> value_force_connects;

	double force_factor = 10.;

	// weighting indicated by the coloring of the mesh
	// alphas gathered by summing up the factors
	// for each corner force
	Eigen::VectorXd connect_alphas;

	// same vars for disconnect
	Eigen::VectorXd disconnect_alphas;

	Eigen::VectorXd edge_lenghts_per_pair;
	Eigen::VectorXd no_seam_constraints_per_pair;
	std::vector<std::pair<int, int>> pair2ind;
	std::map<std::pair<int, int>,int> ind2pair;

	Eigen::VectorXd EsepP_squared_rowwise_sum;
	Eigen::VectorXd EsepP_squared_rowwise_sum_plus_delta;
};