// STL includes
#include <algorithm>

// Optimization lib includes
#include <objective_functions/position.h>
#include <utils/utils.h>

Position::Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
	ObjectiveFunction(objective_function_data_provider, "Position"),
	constrained_vertices_count_(0)
{
	Initialize();
}

Position::~Position()
{

}

void Position::AddConstrainedVertex(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position)
{
	std::lock_guard<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) == im_vi_2_ci_.end())
	{
		auto constrained_index = constrained_vertices_count_;
		constrained_vertices_count_++;
		x_constrained_.conservativeResize(constrained_vertices_count_, 2);
		x_constrained_initial_.conservativeResize(constrained_vertices_count_, 2);
		im_vi_2_ci_[vertex_index] = constrained_index;
		x_constrained_.row(constrained_index) = vertex_position;
		x_constrained_initial_.row(constrained_index) = vertex_position;
	}
}

void Position::UpdateConstrainedVertexPosition(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position)
{
	std::lock_guard<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) != im_vi_2_ci_.end())
	{
		x_constrained_.row(im_vi_2_ci_[vertex_index]) = vertex_position;
	}
}

void Position::OffsetConstrainedVertexPosition(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_offset, const OffsetType offset_type)
{
	std::lock_guard<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) != im_vi_2_ci_.end())
	{
		Eigen::Vector2d relative_position;
		switch (offset_type)
		{
		case OffsetType::RELATIVE_TO_CURRENT:
			relative_position = x_constrained_.row(im_vi_2_ci_[vertex_index]);
			break;

		case OffsetType::RELATIVE_TO_INITIAL:
			relative_position = x_constrained_initial_.row(im_vi_2_ci_[vertex_index]);
			break;
		}

		x_constrained_.row(im_vi_2_ci_[vertex_index]) = relative_position + vertex_offset;
	}
}

Eigen::Vector2d Position::GetConstrainedVertexPosition(Eigen::DenseIndex vertex_index)
{
	std::lock_guard<std::mutex> lock(m_);
	return x_constrained_.row(im_vi_2_ci_[vertex_index]);
}

void Position::RemoveConstrainedVertex(Eigen::DenseIndex vertex_index)
{
	std::lock_guard<std::mutex> lock(m_);
	if (im_vi_2_ci_.find(vertex_index) != im_vi_2_ci_.end())
	{
		auto constrained_index = im_vi_2_ci_[vertex_index];
		constrained_vertices_count_--;
		im_vi_2_ci_.erase(vertex_index);
		Utils::RemoveRow<Eigen::MatrixX2d>(x_constrained_, constrained_index);
		Utils::RemoveRow<Eigen::MatrixX2d>(x_constrained_initial_, constrained_index);
	}
}

bool Position::IsValid()
{
	return constrained_vertices_count_ > 0;
}

void Position::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.resize(variables_count_);
	jj.resize(variables_count_);

	// TODO: No need to fill the whole main diagonal, but only the relevant indices of vertices that were constrained
	for (int i = 0; i < variables_count_; i++)
	{
		ii[i] = i;
		jj[i] = i;
	}

	ss = std::vector<double>(ii.size(), 0);
}

void Position::CalculateValue(const Eigen::VectorXd& x, double& f)
{
	if (im_vi_2_ci_.size() > 0)
	{
		f = x_diff_.squaredNorm();
	}
}

void Position::CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g)
{
	if (im_vi_2_ci_.size() > 0)
	{
		g.setZero();
		Eigen::DenseIndex current_vertex_index;
		for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
		{
			g(image_vertex_index) = 2 * x_diff_(constrained_index, 0);
			g(image_vertex_index + image_vertices_count_) = 2 * x_diff_(constrained_index, 1);
		}
	}
}

void Position::CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss)
{
	if (im_vi_2_ci_.size() > 0)
	{
		std::fill(ss.begin(), ss.end(), 0);
		Eigen::DenseIndex current_vertex_index;
		for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
		{
			ss[image_vertex_index] = 2;
			ss[image_vertex_index + image_vertices_count_] = 2;
		}
	}
}

void Position::PreUpdate(const Eigen::VectorXd& x)
{
	if (im_vi_2_ci_.size() > 0)
	{
		x_current_.resize(constrained_vertices_count_, 2);
		Eigen::DenseIndex current_vertex_index;
		for (const auto& [image_vertex_index, constrained_index] : im_vi_2_ci_)
		{
			x_current_(constrained_index, 0) = x(image_vertex_index);
			x_current_(constrained_index, 1) = x(image_vertex_index + image_vertices_count_);
		}

		x_diff_ = x_constrained_ - x_current_;
	}
}