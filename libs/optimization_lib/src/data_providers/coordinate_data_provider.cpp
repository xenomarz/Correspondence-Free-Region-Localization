// Optimization lib includes
#include <data_providers/coordinate_data_provider.h>

CoordinateDataProvider::CoordinateDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex_index, const RDS::CoordinateType coordinate_type) :
	DataProvider(mesh_data_provider),
	vertex_index_(vertex_index),
	coordinate_type_(coordinate_type)
{
	switch(coordinate_type)
	{
	case RDS::CoordinateType::X:
		sparse_variable_index_ = mesh_data_provider_->GetXVariableIndex(vertex_index);
		break;
	case RDS::CoordinateType::Y:
		sparse_variable_index_ = mesh_data_provider_->GetYVariableIndex(vertex_index);
		break;
	}
}

CoordinateDataProvider::~CoordinateDataProvider()
{
	
}

void CoordinateDataProvider::Update(const Eigen::VectorXd& x)
{
	coordinate_value_ = x.coeff(sparse_variable_index_);
}

void CoordinateDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}

double CoordinateDataProvider::GetCoordinateValue() const
{
	return coordinate_value_;
}

RDS::SparseVariableIndex CoordinateDataProvider::GetSparseVariableIndex() const
{
	return sparse_variable_index_;
}