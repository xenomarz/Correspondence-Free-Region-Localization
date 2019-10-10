// STL includes
#include <algorithm>

// Optimization lib includes
#include <objective_functions/position_objective.h>
#include <utils/utils.h>

Position::Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider) :
	ObjectiveFunction(objective_function_data_provider, "Position")
{
	Initialize();
}

Position::~Position()
{

}

void Position::AddConstrainedVertex(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position)
{
	AddConstrainedVertices(std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>{std::make_pair(vertex_index, vertex_position)});
}

void Position::AddConstrainedVertices(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_position_pairs)
{
	std::lock_guard<std::mutex> lock(m_);
	uint32_t internal_index;
	uint32_t external_index;
	for (uint32_t i = 0; i < index_position_pairs.size(); i++)
	{
		internal_index = ei_2_cv_data.size();
		external_index = index_position_pairs[i].first;
		ei_2_cv_data.insert({ external_index, std::make_unique<ConstrainedVertexData>(internal_index, external_index, index_position_pairs[i].second) });
	}

	ResetConstraintsMatrices();
}

void Position::UpdateConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position)
{
	UpdateConstrainedVerticesPositions(std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>{std::make_pair(vertex_index, vertex_position)});
}

void Position::UpdateConstrainedVerticesPositions(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_position_pairs)
{
	std::lock_guard<std::mutex> lock(m_);
	for (auto& index_position_pair : index_position_pairs)
	{
		auto external_index = index_position_pair.first;
		if (ei_2_cv_data.contains(external_index))
		{
			auto& constrained_vertex_data = ei_2_cv_data[external_index];
			auto new_position = index_position_pair.second;
			x_constrained_.row(constrained_vertex_data->GetInternalIndex()) = new_position;
			constrained_vertex_data->SetCurrentPosition(new_position);
		}
	}
}

void Position::OffsetConstrainedVertexPosition(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_offset, const OffsetType offset_type)
{
	OffsetConstrainedVerticesPositions(std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>{std::make_pair(vertex_index, vertex_offset)}, offset_type);
}

void Position::OffsetConstrainedVerticesPositions(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_offset_pairs, const OffsetType offset_type)
{
	std::lock_guard<std::mutex> lock(m_);
	for (auto& index_offset_pair : index_offset_pairs)
	{
		auto external_index = index_offset_pair.first;
		if (ei_2_cv_data.contains(external_index))
		{
			auto& constrained_vertex_data = ei_2_cv_data[external_index];
			Eigen::Vector2d relative_position;
			switch (offset_type)
			{
			case OffsetType::RELATIVE_TO_CURRENT:
				relative_position = constrained_vertex_data->GetCurrentPosition();
				break;

			case OffsetType::RELATIVE_TO_INITIAL:
				relative_position = constrained_vertex_data->GetInitialPosition();
				break;
			}

			auto new_position = relative_position + index_offset_pair.second;
			x_constrained_.row(constrained_vertex_data->GetInternalIndex()) = new_position;
			constrained_vertex_data->SetCurrentPosition(new_position);
		}
	}
}

bool Position::GetConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, Eigen::Vector2d& vertex_position)
{
	std::lock_guard<std::mutex> lock(m_);
	if (ei_2_cv_data.contains(vertex_index))
	{
		vertex_position = ei_2_cv_data[vertex_index]->GetCurrentPosition();
		return true;
	}

	return false;
}

void Position::RemoveConstrainedVertex(Eigen::DenseIndex vertex_index)
{
	RemoveConstrainedVertices(std::vector<Eigen::DenseIndex>{vertex_index});
}

void Position::RemoveConstrainedVertices(const std::vector<Eigen::DenseIndex>& vertices_indices)
{
	std::lock_guard<std::mutex> lock(m_);
	for (uint32_t i = 0; i < vertices_indices.size(); i++)
	{
		ei_2_cv_data.erase(vertices_indices[i]);
	}

	ResetConstraintsMatrices();
}

void Position::ResetConstrainedVertex(const Eigen::DenseIndex vertex_index)
{
	ResetConstrainedVertices(std::vector<Eigen::DenseIndex>{vertex_index});
}

void Position::ResetConstrainedVertices(const std::vector<Eigen::DenseIndex>& vertices_indices)
{
	std::lock_guard<std::mutex> lock(m_);
	for (uint32_t i = 0; i < vertices_indices.size(); i++)
	{
		auto& constrained_vertex_data = ei_2_cv_data[vertices_indices[i]];
		constrained_vertex_data->SetInitialPosition(constrained_vertex_data->GetCurrentPosition());
	}

	ResetConstraintsMatrices();
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

void Position::CalculateValue(const Eigen::VectorXd& x, double& f, Eigen::VectorXd& f_per_vertex)
{
	if (ei_2_cv_data.size() > 0)
	{
		f = x_diff_.squaredNorm();
	}
	else
	{
		f = 0;
	}
}

void Position::CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g)
{
	g.setZero();
	if (ei_2_cv_data.size() > 0)
	{
		uint32_t internal_index;
		for (const auto& [external_index, constrained_vertex_data] : ei_2_cv_data)
		{
			internal_index = constrained_vertex_data->GetInternalIndex();
			g(external_index) = 2 * x_diff_(internal_index, 0);
			g(external_index + image_vertices_count_) = 2 * x_diff_(internal_index, 1);
		}
	}
}

void Position::CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss)
{
	std::fill(ss.begin(), ss.end(), 0);
	if (ei_2_cv_data.size() > 0)
	{
		for (const auto& [external_index, constrained_vertex_data] : ei_2_cv_data)
		{
			ss[external_index] = 2;
			ss[external_index + image_vertices_count_] = 2;
		}
	}
}

void Position::PreUpdate(const Eigen::VectorXd& x)
{
	uint32_t constrained_vertices_count = ei_2_cv_data.size();
	if (constrained_vertices_count > 0)
	{
		x_current_.resize(constrained_vertices_count, 2);
		uint32_t internal_index;
		for (const auto& [external_index, constrained_vertex_data] : ei_2_cv_data)
		{
			internal_index = constrained_vertex_data->GetInternalIndex();
			x_current_(internal_index, 0) = x(external_index);
			x_current_(internal_index, 1) = x(external_index + image_vertices_count_);
		}

		x_diff_ = x_current_ - x_constrained_;
	}
}

void Position::ResetConstraintsMatrices()
{
	uint32_t constrained_vertices_count = ei_2_cv_data.size();
	if (constrained_vertices_count > 0)
	{
		x_constrained_.resize(constrained_vertices_count, 2);
		x_constrained_initial_.resize(constrained_vertices_count, 2);
		uint32_t internal_index = 0;
		for (auto& [external_index, constrained_vertex_data] : ei_2_cv_data)
		{
			x_constrained_.row(internal_index) = constrained_vertex_data->GetCurrentPosition();
			x_constrained_initial_.row(internal_index) = constrained_vertex_data->GetInitialPosition();
			constrained_vertex_data->SetInternalIndex(internal_index);
			internal_index++;
		}
	}
}

Position::ConstrainedVertexData::ConstrainedVertexData(const uint32_t internal_index, const uint32_t external_index, const Eigen::Vector2d& initial_position) :
	internal_index_(internal_index),
	external_index_(external_index),
	initial_position_(initial_position),
	current_position_(initial_position)
{

}

void Position::ConstrainedVertexData::SetInternalIndex(const uint32_t internal_index)
{
	internal_index_ = internal_index;
}

void Position::ConstrainedVertexData::SetInitialPosition(const Eigen::Vector2d& position)
{
	initial_position_ = position;
}

void Position::ConstrainedVertexData::SetCurrentPosition(const Eigen::Vector2d& position)
{
	current_position_ = position;
}

const uint32_t Position::ConstrainedVertexData::GetInternalIndex() const
{
	return internal_index_;
}

const uint32_t Position::ConstrainedVertexData::GetExternalIndex() const
{
	return external_index_;
}

const Eigen::Vector2d& Position::ConstrainedVertexData::GetInitialPosition() const
{
	return initial_position_;
}

const Eigen::Vector2d& Position::ConstrainedVertexData::GetCurrentPosition() const
{
	return current_position_;
}