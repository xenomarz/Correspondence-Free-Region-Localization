#include <utils/data_providers/data_provider.h>

DataProvider::DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	UpdatableObject(),
	mesh_data_provider_(mesh_data_provider)
{
	
}

DataProvider::~DataProvider()
{
	
}

const MeshDataProvider& DataProvider::GetMeshDataProvider() const
{
	return *mesh_data_provider_;
}