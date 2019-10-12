// Optimization lib includes
#include <objective_functions/barycenter_position_objective.h>
#include <utils/utils.h>

BarycenterPositionObjective::BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<int64_t>& indices, const Eigen::Vector2d& objective_barycenter) :
	PositionObjective(objective_function_data_provider, 2.0 / static_cast<double>(indices.size()), indices.size(), "Barycenter Position Objective"),
	indices_(indices),
	objective_barycenter_(objective_barycenter)
{
	Initialize();
}

BarycenterPositionObjective::BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const Eigen::VectorXi& indices, const Eigen::Vector2d& objective_barycenter) :
	BarycenterPositionObjective(objective_function_data_provider, std::vector<int64_t>(indices.data(), indices.data() + indices.rows()), objective_barycenter)
{

}

BarycenterPositionObjective::~BarycenterPositionObjective()
{

}

void BarycenterPositionObjective::OffsetPositionConstraint(const Eigen::Vector2d& offset)
{
	objective_barycenter_ += offset;
}

void BarycenterPositionObjective::InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{
	ii.resize(vertices_count_);
	jj.resize(vertices_count_);

	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		ii[i] = indices_[i];
		jj[i] = indices_[i];
	}

	ss = std::vector<double>(vertices_count_, 0);
}

void BarycenterPositionObjective::CalculateValue(double& f, Eigen::VectorXd& f_per_vertex)
{
	f = barycenters_diff_.squaredNorm();
}

void BarycenterPositionObjective::CalculateGradient(Eigen::VectorXd& g)
{
	g.setZero();
	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		const auto current_index = indices_[i];
		g(current_index) = coefficient_ * barycenters_diff_(0,0);
		g(current_index + image_vertices_count_) = coefficient_ * barycenters_diff_(1, 0);
	}
}

void BarycenterPositionObjective::CalculateHessian(std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);
	for (uint64_t i = 0; i < vertices_count_; i++)
	{
		const auto current_index = indices_[i];
		ss[current_index] = coefficient_;
		ss[current_index + image_vertices_count_] = coefficient_;
	}
}

void BarycenterPositionObjective::PreUpdate(const Eigen::VectorXd& x)
{
	auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
	Utils::CalculateBarycenter(indices_, X, current_barycenter_);
	barycenters_diff_ = current_barycenter_ - objective_barycenter_;
}
