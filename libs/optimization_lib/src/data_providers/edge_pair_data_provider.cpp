// Optimization lib includes
#include <data_providers/edge_pair_data_provider.h>

EdgePairDataProvider::EdgePairDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::EdgePairDescriptor& edge_pair_descriptor) :
	DataProvider(mesh_data_provider),
	edge_pair_descriptor_(edge_pair_descriptor)
{
	edge1_v1_x_index_ = mesh_data_provider->GetVertexXIndex(edge_pair_descriptor.first.first);
	edge1_v1_y_index_ = mesh_data_provider->GetVertexYIndex(edge_pair_descriptor.first.first);
	edge1_v2_x_index_ = mesh_data_provider->GetVertexXIndex(edge_pair_descriptor.first.second);
	edge1_v2_y_index_ = mesh_data_provider->GetVertexYIndex(edge_pair_descriptor.first.second);

	edge2_v1_x_index_ = mesh_data_provider->GetVertexXIndex(edge_pair_descriptor.second.first);
	edge2_v1_y_index_ = mesh_data_provider->GetVertexYIndex(edge_pair_descriptor.second.first);
	edge2_v2_x_index_ = mesh_data_provider->GetVertexXIndex(edge_pair_descriptor.second.second);
	edge2_v2_y_index_ = mesh_data_provider->GetVertexYIndex(edge_pair_descriptor.second.second);
}

EdgePairDataProvider::~EdgePairDataProvider()
{
	
}

void EdgePairDataProvider::Update(const Eigen::VectorXd& x)
{
	edge1_.coeffRef(0) = x(edge1_v2_x_index_) - x(edge1_v1_x_index_);
	edge1_.coeffRef(1) = x(edge1_v2_y_index_) - x(edge1_v1_y_index_);
	
	edge2_.coeffRef(0) = x(edge2_v2_x_index_) - x(edge2_v1_x_index_);
	edge2_.coeffRef(1) = x(edge2_v2_y_index_) - x(edge2_v1_y_index_);
	
	edge1_x_diff_ = edge1_.coeffRef(0);
	edge1_y_diff_ = edge1_.coeffRef(1);
	edge2_x_diff_ = edge2_.coeffRef(0);
	edge2_y_diff_ = edge2_.coeffRef(1);

	edge1_x_diff_squared_ = edge1_x_diff_ * edge1_x_diff_;
	edge1_y_diff_squared_ = edge1_y_diff_ * edge1_y_diff_;
	edge2_x_diff_squared_ = edge2_x_diff_ * edge2_x_diff_;
	edge2_y_diff_squared_ = edge2_y_diff_ * edge2_y_diff_;

	edge1_squared_norm_ = edge1_x_diff_squared_ + edge1_y_diff_squared_;
	edge2_squared_norm_ = edge2_x_diff_squared_ + edge2_y_diff_squared_;

	edge1_quadrupled_norm_ = edge1_squared_norm_ * edge1_squared_norm_;
	edge2_quadrupled_norm_ = edge2_squared_norm_ * edge2_squared_norm_;
}

const Eigen::Vector2d& EdgePairDataProvider::GetEdge1() const
{
	return edge1_;
}

const Eigen::Vector2d& EdgePairDataProvider::GetEdge2() const
{
	return edge2_;
}

int64_t EdgePairDataProvider::GetEdge1Vertex1XIndex() const
{
	return edge1_v1_x_index_;
}

int64_t EdgePairDataProvider::GetEdge1Vertex1YIndex() const
{
	return edge1_v1_y_index_;
}

int64_t EdgePairDataProvider::GetEdge1Vertex2XIndex() const
{
	return edge1_v2_x_index_;
}

int64_t EdgePairDataProvider::GetEdge1Vertex2YIndex() const
{
	return edge1_v2_y_index_;
}

int64_t EdgePairDataProvider::GetEdge2Vertex1XIndex() const
{
	return edge2_v1_x_index_;
}

int64_t EdgePairDataProvider::GetEdge2Vertex1YIndex() const
{
	return edge2_v1_y_index_;
}

int64_t EdgePairDataProvider::GetEdge2Vertex2XIndex() const
{
	return edge2_v2_x_index_;
}

int64_t EdgePairDataProvider::GetEdge2Vertex2YIndex() const
{
	return edge2_v2_y_index_;
}

double EdgePairDataProvider::GetEdge1XDiff() const
{
	return edge1_x_diff_;
}

double EdgePairDataProvider::GetEdge1YDiff() const
{
	return edge1_y_diff_;
}

double EdgePairDataProvider::GetEdge2XDiff() const
{
	return edge2_x_diff_;
}

double EdgePairDataProvider::GetEdge2YDiff() const
{
	return edge2_y_diff_;
}

double EdgePairDataProvider::GetEdge1XSquaredDiff() const
{
	return edge1_x_diff_squared_;
}

double EdgePairDataProvider::GetEdge1YSquaredDiff() const
{
	return edge1_y_diff_squared_;
}

double EdgePairDataProvider::GetEdge2XSquaredDiff() const
{
	return edge2_x_diff_squared_;
}

double EdgePairDataProvider::GetEdge2YSquaredDiff() const
{
	return edge2_y_diff_squared_;
}

double EdgePairDataProvider::GetEdge1SquaredNrom() const
{
	return edge1_squared_norm_;
}

double EdgePairDataProvider::GetEdge2SquaredNrom() const
{
	return edge2_squared_norm_;
}

double EdgePairDataProvider::GetEdge1QuadrupledNrom() const
{
	return edge1_quadrupled_norm_;
}

double EdgePairDataProvider::GetEdge2QuadrupledNrom() const
{
	return edge2_quadrupled_norm_;
}