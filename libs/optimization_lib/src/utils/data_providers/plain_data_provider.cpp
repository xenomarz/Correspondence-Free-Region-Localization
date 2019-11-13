#include "./utils/data_providers/plain_data_provider.h"

PlainDataProvider::PlainDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	DataProvider(mesh_data_provider)
{
	
}

PlainDataProvider::PlainDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const Eigen::VectorXd& x0) :
	DataProvider(mesh_data_provider),
	x_(x0)
{
	
}

PlainDataProvider::~PlainDataProvider()
{
	
}

void PlainDataProvider::Update(const Eigen::VectorXd& x)
{
	x_ = x;
}