// Optimization lib includes
#include <data_providers/data_provider.h>

DataProvider::DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	UpdatableObject(mesh_data_provider)
{
	variables_.resize(mesh_data_provider->GetVariablesCount());
}

DataProvider::~DataProvider()
{
	
}

const Eigen::SparseVector<double>& DataProvider::GetVariables() const
{
	return variables_;
}

double DataProvider::GetVariableValue(const RDS::SparseVariableIndex variable_index) const
{
	return variables_.coeff(variable_index);
}

double DataProvider::GetVariableValue(const RDS::VertexIndex vertex_index, const CoordinateType coordinate_type) const
{
	switch (coordinate_type)
	{
	case CoordinateType::X:
		return variables_.coeff(mesh_data_provider_->GetXVariableIndex(vertex_index));
	case CoordinateType::Y:
		return variables_.coeff(mesh_data_provider_->GetYVariableIndex(vertex_index));
	}
	
	throw std::exception("DataProvider::GetCoordinateValue - Unknown coordinate type");
}