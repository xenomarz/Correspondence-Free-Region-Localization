#include <utils/data_providers/data_provider.h>

DataProvider::DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	mesh_data_provider_(mesh_data_provider)
{
	
}

DataProvider::~DataProvider()
{
	
}