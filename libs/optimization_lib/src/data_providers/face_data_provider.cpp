// Optimization lib includes
#include <data_providers/face_data_provider.h>

FaceDataProvider::FaceDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::Face& face) :
	DataProvider(mesh_data_provider),
	face_(face)
{

}

FaceDataProvider::~FaceDataProvider()
{

}

void FaceDataProvider::Update(const Eigen::VectorXd& x)
{
	barycenter_ = Utils::CalculateBarycenter(face_, x);
}

void FaceDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}

const RDS::Face& FaceDataProvider::GetFace() const
{
	return face_;
}

const Eigen::VectorXd& FaceDataProvider::GetBarycenter() const
{
	return barycenter_;
}