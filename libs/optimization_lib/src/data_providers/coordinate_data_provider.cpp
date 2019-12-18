// Optimization lib includes
#include <data_providers/coordinate_data_provider.h>

CoordinateDataProvider::CoordinateDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex_index, const CoordinateType coordinate_type) :
	DataProvider(mesh_data_provider),
	vertex_index_(vertex_index),
	coordinate_type_(coordinate_type)
{
	switch(coordinate_type)
	{
	case CoordinateType::X:
		sparse_variable_index_ = mesh_data_provider_->GetXVariableIndex(vertex_index);
		break;
	case CoordinateType::Y:
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
double CoordinateDataProvider::GetCoordinateValue() const
{
	return coordinate_value_;
}

RDS::SparseVariableIndex CoordinateDataProvider::GetSparseVariableIndex() const
{
	return sparse_variable_index_;
}