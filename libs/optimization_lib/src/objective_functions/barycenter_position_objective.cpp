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
	ii.resize(objective_variables_count_);
	jj.resize(objective_variables_count_);

	int64_t vertex_index;
	int64_t vertex_index_shifted;
	int64_t i_shifted;
	
	for (uint64_t i = 0; i < objective_vertices_count_; i++)
	{
		vertex_index = indices_[i];
		vertex_index_shifted = vertex_index + image_vertices_count_;
		i_shifted = i + objective_vertices_count_;
		
		ii[i] = vertex_index;
		jj[i] = vertex_index;
		
		ii[i_shifted] = vertex_index_shifted;
		jj[i_shifted] = vertex_index_shifted;
	}

	ss = std::vector<double>(objective_variables_count_, 0);
}

void BarycenterPositionObjective::CalculateValue(double& f, Eigen::VectorXd& f_per_vertex)
{
	f = 0;
	//f = barycenters_diff_.squaredNorm();
}

void BarycenterPositionObjective::CalculateGradient(Eigen::VectorXd& g)
{
	g.setZero();
	//for (uint64_t i = 0; i < objective_vertices_count_; i++)
	//{
	//	const auto current_index = indices_[i];
	//	g(current_index) = (2.0f / 3.0f) * barycenters_diff_(0,0);
	//	g(current_index + image_vertices_count_) = (2.0f / 3.0f) * barycenters_diff_(1, 0);
	//}
}

void BarycenterPositionObjective::CalculateHessian(std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);

	//int64_t vertex_index;
	//for (uint64_t i = 0; i < objective_vertices_count_; i++)
	//{
	//	//vertex_index = indices_[i];
	//	ss[i] = 2.0f / 9.0f;
	//	ss[i + objective_vertices_count_] = 2.0f / 9.0f;
	//}
}

void BarycenterPositionObjective::PreUpdate(const Eigen::VectorXd& x)
{
	//auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
 //	Utils::CalculateBarycenter(indices_, X, current_barycenter_);
	//barycenters_diff_ = current_barycenter_ - objective_barycenter_;
}
