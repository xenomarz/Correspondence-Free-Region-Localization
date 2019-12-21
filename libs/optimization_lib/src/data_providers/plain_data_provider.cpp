// Optimization lib includes
#include <data_providers/plain_data_provider.h>

PlainDataProvider::PlainDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	DataProvider(mesh_data_provider)
{
	
}

PlainDataProvider::~PlainDataProvider()
{
	
}

void PlainDataProvider::Update(const Eigen::VectorXd& x)
{
	x_ = x;
}

void PlainDataProvider::Update(const Eigen::VectorXd& x, const UpdateOptions update_options)
{
	Update(x);
}

const Eigen::VectorXd& PlainDataProvider::GetX() const
{
	return x_;
}