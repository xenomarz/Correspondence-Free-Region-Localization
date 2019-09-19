// STL includes
#include <algorithm>

// Optimization lib includes
#include <objective_functions/position.h>
#include <utils/utils.h>

Position::Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
	ObjectiveFunction(objective_function_data_provider, "Position"),
	constrained_vertices_count_(0)
{

}

Position::~Position()
{

}

void Position::AddConstrainedVertex(Eigen::DenseIndex vertex_index, Eigen::Vector2d vertex_position)
{
	std::unique_lock<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) == im_vi_2_ci_.end())
	{
		auto constrained_index = constrained_vertices_count_;
		constrained_vertices_count_++;
		x_constrained_.conservativeResize(constrained_vertices_count_, 2);
		im_vi_2_ci_[vertex_index] = constrained_index;
		x_constrained_.row(constrained_index) = vertex_position;
	}
}

void Position::UpdateConstrainedVertex(Eigen::DenseIndex vertex_index, Eigen::Vector2d vertex_position)
{
	std::unique_lock<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) != im_vi_2_ci_.end())
	{
		x_constrained_.row(im_vi_2_ci_[vertex_index]) = vertex_position;
	}
}

void Position::RemoveConstrainedVertex(Eigen::DenseIndex vertex_index)
{
	std::unique_lock<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) != im_vi_2_ci_.end())
	{
		auto constrained_index = im_vi_2_ci_[vertex_index];
		constrained_vertices_count_--;
		im_vi_2_ci_.erase(vertex_index);
		Utils::RemoveRow<Eigen::MatrixX2d>(x_constrained_, constrained_index);
	}
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
	for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
	{
		g(image_vertex_index) = 2 * x_diff_(constrained_index, 0);
		g(image_vertex_index + image_vertices_count_) = 2 * x_diff_(constrained_index, 1);
	}
}

void Position::CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);
	Eigen::DenseIndex current_vertex_index;
	for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
	{
		ss[image_vertex_index] = 2;
		ss[image_vertex_index + image_vertices_count_] = 2;
	}
}

void Position::PreUpdate(const Eigen::MatrixX2d& x)
{
	x_current_.resize(2, constrained_vertices_count_);
	Eigen::DenseIndex current_vertex_index;
	for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
	{
		x_current_(constrained_index, 0) = x(image_vertex_index, 0);
		x_current_(constrained_index, 1) = x(image_vertex_index, 1);
	}

	x_diff_ = x_constrained_ - x_current_;
}