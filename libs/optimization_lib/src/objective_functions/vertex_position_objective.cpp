// Optimization lib includes
#include <objective_functions/vertex_position_objective.h>
#include <utils/utils.h>

VertexPositionObjective::VertexPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::pair<int64_t, Eigen::Vector2d>>& index_vertex_pairs) :
	PositionObjective(objective_function_data_provider, 2.0, index_vertex_pairs.size(), "Barycenter Position Objective"),
	index_vertex_pairs_(index_vertex_pairs)
{
	X_objective_.resize(vertices_count_, 2);
	for(int64_t i = 0; i < vertices_count_; i++)
	{
		X_objective_.row(i) = index_vertex_pairs[i].second;
	}
	Initialize();
}

VertexPositionObjective::~VertexPositionObjective()
{

}

void VertexPositionObjective::OffsetPositionConstraint(const Eigen::Vector2d& offset)
{
	X_objective_ = X_objective_.rowwise() + offset.transpose();
}

void VertexPositionObjective::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.resize(vertices_count_);
	jj.resize(vertices_count_);

	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		ii[i] = index_vertex_pairs_[i].first;
		jj[i] = index_vertex_pairs_[i].first;
	}

	ss = std::vector<double>(vertices_count_, 0);
}

void VertexPositionObjective::CalculateValue(double& f, Eigen::VectorXd& f_per_vertex)
{
	f = X_diff_.squaredNorm();
}

void VertexPositionObjective::CalculateGradient(Eigen::VectorXd& g)
{
	g.setZero();
	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		const auto current_index = index_vertex_pairs_[i].first;
		g(current_index) = coefficient_ * X_diff_(i, 0);
		g(current_index + image_vertices_count_) = coefficient_ * X_diff_(i, 1);
	}
}

void VertexPositionObjective::CalculateHessian(std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);
	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		const auto current_index = index_vertex_pairs_[i].first;
		ss[current_index] = coefficient_;
		ss[current_index + image_vertices_count_] = coefficient_;
	}
}

void VertexPositionObjective::PreUpdate(const Eigen::VectorXd& x)
{
	X_current_ = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
	X_diff_ = X_current_ - X_objective_;
}
