// Optimization lib includes
#include <data_providers/edge_pair_data_provider.h>

EdgePairDataProvider::EdgePairDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::EdgePairDescriptor& edge_pair_descriptor) :
	DataProvider(mesh_data_provider),
	edge_pair_descriptor_(edge_pair_descriptor)
{
	edge1_v1_x_index_ = mesh_data_provider->GetXVariableIndex(edge_pair_descriptor.first.first);
	edge1_v1_y_index_ = mesh_data_provider->GetYVariableIndex(edge_pair_descriptor.first.first);
	edge1_v2_x_index_ = mesh_data_provider->GetXVariableIndex(edge_pair_descriptor.first.second);
	edge1_v2_y_index_ = mesh_data_provider->GetYVariableIndex(edge_pair_descriptor.first.second);

	edge2_v1_x_index_ = mesh_data_provider->GetXVariableIndex(edge_pair_descriptor.second.first);
	edge2_v1_y_index_ = mesh_data_provider->GetYVariableIndex(edge_pair_descriptor.second.first);
	edge2_v2_x_index_ = mesh_data_provider->GetXVariableIndex(edge_pair_descriptor.second.second);
	edge2_v2_y_index_ = mesh_data_provider->GetYVariableIndex(edge_pair_descriptor.second.second);

	image_edge_1_index_ = mesh_data_provider->GetImageEdgeIndex(edge_pair_descriptor.first);
	image_edge_2_index_ = mesh_data_provider->GetImageEdgeIndex(edge_pair_descriptor.second);
	domain_edge_index_ = mesh_data_provider->GetDomainEdgeIndex(edge_pair_descriptor.first);
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

void EdgePairDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}