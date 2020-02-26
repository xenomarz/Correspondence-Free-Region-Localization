// Optimization lib includes
#include <data_providers/face_data_provider.h>

FaceDataProvider::FaceDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::FaceDescriptor& face_descriptor) :
	DataProvider(mesh_data_provider),
	face_descriptor_(face_descriptor)
{

}

FaceDataProvider::~FaceDataProvider()
{

}

void FaceDataProvider::Update(const Eigen::VectorXd& x)
{
	barycenter_ = Utils::CalculateBarycenter(face_descriptor_, x);
}

void FaceDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
{
	Update(x);
}

const RDS::FaceDescriptor& FaceDataProvider::GetFaceDescriptor() const
{
	return face_descriptor_;
}

const Eigen::VectorXd& FaceDataProvider::GetBarycenter() const
{
	return barycenter_;
}