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

public:
	enum class SeparationEnergy { 
		LOG,
		QUADRATIC,
		FLAT_LOG,
		QUOTIENT,
		QUOTIENT_NEW
	};


	void init(int n);
	void value(const Eigen::MatrixX2d& X, double& f);
	void gradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g);
	void hessian(const Eigen::MatrixX2d& X);

	void find_single_hessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h);
	void update_alphas(const Eigen::MatrixXd& weights, double max_possible);

	Eigen::VectorXd GetVertexEnergy();

	Eigen::SparseMatrix<double> EVvar1, EVvar2, Esep, Esept, V2V, V2Vt;
	Eigen::SparseMatrix<double> C2C; //Corner to corner
	Eigen::MatrixX2d EsepP;

	double Lsep = 1.0, delta = 1.0;
	SeparationEnergy sepEType = SeparationEnergy::QUOTIENT_NEW;

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

private:
	Eigen::VectorXd EsepP_squared_rowwise_sum;
	Eigen::VectorXd EsepP_squared_rowwise_sum_plus_delta;
	

	void flat_log_single_hessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h);
	void make_spd(Eigen::Matrix4d& h);
	void add_to_global_hessian(const Eigen::Matrix4d& sh, int idx_xi, int idx_xj, int n, std::list<Eigen::Triplet<double>>& htriplets);
	inline int sign(double val);
	inline double dirac(double val);

	void prepare_hessian(int n);

	Eigen::VectorXd vertex_energy;
};