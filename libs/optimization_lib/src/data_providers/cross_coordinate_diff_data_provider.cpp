// Optimization lib includes
#include <data_providers/cross_coordinate_diff_data_provider.h>

CrossCoordinateDiffDataProvider::CrossCoordinateDiffDataProvider(
	const std::shared_ptr<MeshDataProvider>& mesh_data_provider,
	const RDS::VertexIndex edge1_vertex1_index,
	const RDS::VertexIndex edge1_vertex2_index,
	const RDS::VertexIndex edge2_vertex1_index,
	const RDS::VertexIndex edge2_vertex2_index,
	const RDS::CoordinateType coordinate_type) :
	DataProvider(mesh_data_provider),
	edge1_vertex1_index_(edge1_vertex1_index),
	edge1_vertex2_index_(edge1_vertex2_index),
	edge2_vertex1_index_(edge2_vertex1_index),
	edge2_vertex2_index_(edge2_vertex2_index),
	coordinate_type_(coordinate_type)
{
	switch(coordinate_type)
	{
	case RDS::CoordinateType::X:
		edge1_variable1_index_ = mesh_data_provider_->GetXVariableIndex(edge1_vertex1_index);
		edge1_variable2_index_ = mesh_data_provider_->GetXVariableIndex(edge1_vertex2_index);
		edge2_variable1_index_ = mesh_data_provider_->GetXVariableIndex(edge2_vertex1_index);
		edge2_variable2_index_ = mesh_data_provider_->GetXVariableIndex(edge2_vertex2_index);
		break;
	case RDS::CoordinateType::Y:
		edge1_variable1_index_ = mesh_data_provider_->GetYVariableIndex(edge1_vertex1_index);
		edge1_variable2_index_ = mesh_data_provider_->GetYVariableIndex(edge1_vertex2_index);
		edge2_variable1_index_ = mesh_data_provider_->GetYVariableIndex(edge2_vertex1_index);
		edge2_variable2_index_ = mesh_data_provider_->GetYVariableIndex(edge2_vertex2_index);
		break;
	}
}

CrossCoordinateDiffDataProvider::~CrossCoordinateDiffDataProvider()
{
	
}

void CrossCoordinateDiffDataProvider::Update(const Eigen::VectorXd& x)
{
	coordinate1_diff_value_ = x.coeff(edge1_variable1_index_) - x.coeff(edge2_variable1_index_);
	coordinate2_diff_value_ = x.coeff(edge1_variable2_index_) - x.coeff(edge2_variable2_index_);
	cross_coordinate_diff_value_ = coordinate1_diff_value_ - coordinate2_diff_value_;
	cross_coordinate_diff_value_squared_ = cross_coordinate_diff_value_ * cross_coordinate_diff_value_;
}

void CrossCoordinateDiffDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}

double CrossCoordinateDiffDataProvider::GetCoordinate1DiffValue() const
{
	return coordinate1_diff_value_;
}

double CrossCoordinateDiffDataProvider::GetCoordinate2DiffValue() const
{
	return coordinate2_diff_value_;
}

double CrossCoordinateDiffDataProvider::GetCrossCoordinateDiffValue() const
{
	return cross_coordinate_diff_value_;
}

double CrossCoordinateDiffDataProvider::GetCrossCoordinateDiffValueSquared() const
{
	return cross_coordinate_diff_value_squared_;
}

RDS::SparseVariableIndex CrossCoordinateDiffDataProvider::GetEdge1Variable1Index() const
{
	return edge1_variable1_index_;
}

RDS::SparseVariableIndex CrossCoordinateDiffDataProvider::GetEdge1Variable2Index() const
{
	return edge1_variable2_index_;
}

RDS::SparseVariableIndex CrossCoordinateDiffDataProvider::GetEdge2Variable1Index() const
{
	return edge2_variable1_index_;
}

RDS::SparseVariableIndex CrossCoordinateDiffDataProvider::GetEdge2Variable2Index() const
{
	return edge2_variable2_index_;
}