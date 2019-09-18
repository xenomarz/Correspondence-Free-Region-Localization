// STL includes
#include <algorithm>

// Optimization lib includes
#include <objective_functions/position.h>

Position::Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
	ObjectiveFunction(objective_function_data_provider, "Position"),
	constrained_vertices_count_(0)
{

}

Position::~Position()
{

}

void Position::UpdateConstrainedPositionsMatrix()
{
	constrained_vertices_count_ = constrained_vertices_.size();
	x_constrained_.resize(2, constrained_vertices_count_);
	for (Eigen::DenseIndex i = 0; i < constrained_vertices_count_; i++)
	{
		x_constrained_(i, 0) = constrained_vertices_[i].second(0);
		x_constrained_(i, 1) = constrained_vertices_[i].second(1);
	}
}

void Position::AddConstrainedVertex(Eigen::DenseIndex vertex_index, Eigen::Vector2d vertex_position)
{
	std::unique_lock<std::mutex> lock(m_);
	constrained_vertices_.push_back(std::make_pair(vertex_index, vertex_position));
	UpdateConstrainedPositionsMatrix();
}

void Position::RemoveConstrainedVertex(Eigen::DenseIndex vertex_index)
{
	std::unique_lock<std::mutex> lock(m_);
	std::remove_if(constrained_vertices_.begin(), constrained_vertices_.end(), [&vertex_index](const ConstrainedVertex& constrained_vertex) {
		return constrained_vertex.first == vertex_index;
	});
	UpdateConstrainedPositionsMatrix();
}

void Position::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.resize(variables_count_);
	jj.resize(variables_count_);
	for (int i = 0; i < variables_count_; i++)
	{
		ii[i] = i;
		jj[i] = i;
	}

	ss = std::vector<double>(ii.size(), 0);
}

void Position::CalculateValue(const Eigen::MatrixX2d& X, double& f)
{
	f = x_diff_.squaredNorm();
}

void Position::CalculateGradient(const Eigen::MatrixX2d& x, Eigen::VectorXd& g)
{
	g.setZero();
	Eigen::DenseIndex current_vertex_index;
	for (Eigen::DenseIndex i = 0; i < constrained_vertices_count_; i++)
	{
		current_vertex_index = constrained_vertices_[i].first;
		g(current_vertex_index) = 2 * x_diff_(i, 0);
		g(current_vertex_index + image_vertices_count_) = 2 * x_diff_(i, 1);
	}
}

void Position::CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);
	Eigen::DenseIndex current_vertex_index;
	for (Eigen::DenseIndex i = 0; i < constrained_vertices_count_; i++)
	{
		current_vertex_index = constrained_vertices_[i].first;
		ss[current_vertex_index] = 2;
		ss[current_vertex_index + image_vertices_count_] = 2;
	}
}

void Position::PreUpdate(const Eigen::MatrixX2d& x)
{
	x_current_.resize(2, constrained_vertices_count_);
	Eigen::DenseIndex current_vertex_index;
	for (Eigen::DenseIndex i = 0; i < constrained_vertices_count_; i++)
	{
		current_vertex_index = constrained_vertices_[i].first;
		x_current_(i, 0) = x(current_vertex_index, 0);
		x_current_(i, 1) = x(current_vertex_index, 1);
	}

	x_diff_ = x_constrained_ - x_current_;
}