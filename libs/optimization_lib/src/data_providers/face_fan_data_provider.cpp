// Optimization lib includes
#include <data_providers/face_fan_data_provider.h>

FaceFanDataProvider::FaceFanDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::FaceFan& face_fan) :
	DataProvider(mesh_data_provider),
	face_fan_(face_fan)
{
	domain_vertex_index_ = this->mesh_data_provider_->GetDomainVertexIndex(face_fan_[0].first);
}

FaceFanDataProvider::~FaceFanDataProvider()
{

}

void FaceFanDataProvider::Update(const Eigen::VectorXd& x)
{
	double accumulated_angle = 0;
	const auto face_fan_count = face_fan_.size();
	for (std::size_t face_fan_index = 0; face_fan_index < face_fan_count; face_fan_index++)
	{
		RDS::FaceFanSlice face_fan_slice = face_fan_[face_fan_index];
		
		RDS::VertexIndex v0_index = face_fan_slice.first;
		RDS::VertexIndex v1_index = face_fan_slice.second.first;
		RDS::VertexIndex v2_index = face_fan_slice.second.second;

		Eigen::Vector2d v0;
		Eigen::Vector2d v1;
		Eigen::Vector2d v2;

		v0.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetXVariableIndex(v0_index));
		v0.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetYVariableIndex(v0_index));

		v1.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetXVariableIndex(v1_index));
		v1.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetYVariableIndex(v1_index));

		v2.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetXVariableIndex(v2_index));
		v2.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetYVariableIndex(v2_index));

		Eigen::Vector2d e1 = v1 - v0;
		Eigen::Vector2d e2 = v2 - v0;

		e1.normalize();
		e2.normalize();

		const double current_angle = abs(acos(e1.dot(e2)));
		accumulated_angle += current_angle;
	}

	angle_ = accumulated_angle;
}

void FaceFanDataProvider::Update(const Eigen::VectorXd& x, const UpdateOptions update_options)
{
	Update(x);
}

double FaceFanDataProvider::GetAngle() const
{
	return angle_;
}

const RDS::FaceFan& FaceFanDataProvider::GetFaceFan() const
{
	return face_fan_;
}

RDS::VertexIndex FaceFanDataProvider::GetDomainVertexIndex() const
{
	return domain_vertex_index_;
}