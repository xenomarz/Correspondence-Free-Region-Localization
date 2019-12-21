// Optimization lib includes
#include <data_providers/empty_data_provider.h>

EmptyDataProvider::EmptyDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	DataProvider(mesh_data_provider)
{

}

EmptyDataProvider::~EmptyDataProvider()
{

}

void EmptyDataProvider::Update(const Eigen::VectorXd& x)
{
	// Empty implementation
}

void EmptyDataProvider::Update(const Eigen::VectorXd& x, const UpdateOptions update_options)
{
	Update(x);
}