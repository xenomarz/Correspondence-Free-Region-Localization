// Optimization lib includes
#include <data_providers/coordinate_diff_data_provider.h>

CoordinateDiffDataProvider::CoordinateDiffDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex1_index, const RDS::VertexIndex vertex2_index, const RDS::CoordinateType coordinate_type) :
	DataProvider(mesh_data_provider),
	vertex1_index_(vertex1_index),
	vertex2_index_(vertex2_index),
	coordinate_type_(coordinate_type)
{
	switch(coordinate_type)
	{
	case RDS::CoordinateType::X:
		sparse_variable1_index_ = mesh_data_provider_->GetXVariableIndex(vertex1_index);
		sparse_variable2_index_ = mesh_data_provider_->GetXVariableIndex(vertex2_index);
		break;
	case RDS::CoordinateType::Y:
		sparse_variable1_index_ = mesh_data_provider_->GetYVariableIndex(vertex1_index);
		sparse_variable2_index_ = mesh_data_provider_->GetYVariableIndex(vertex2_index);
		break;
	}
}

CoordinateDiffDataProvider::~CoordinateDiffDataProvider()
{
	
}

void CoordinateDiffDataProvider::Update(const Eigen::VectorXd& x)
{
	coordinate_diff_value_ = x.coeff(sparse_variable1_index_) - x.coeff(sparse_variable2_index_);
}

void CoordinateDiffDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}

double CoordinateDiffDataProvider::GetCoordinateDiffValue() const
{
	return coordinate_diff_value_;
}

RDS::SparseVariableIndex CoordinateDiffDataProvider::GetSparseVariable1Index() const
{
	return sparse_variable1_index_;
}

RDS::SparseVariableIndex CoordinateDiffDataProvider::GetSparseVariable2Index() const
{
	return sparse_variable2_index_;
}