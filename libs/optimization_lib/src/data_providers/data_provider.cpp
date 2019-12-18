// Optimization lib includes
#include <data_providers/data_provider.h>

DataProvider::DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	UpdatableObject(mesh_data_provider)
{
	
}

DataProvider::~DataProvider()
{
	
}