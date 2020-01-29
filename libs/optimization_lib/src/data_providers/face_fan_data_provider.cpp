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

		RDS::VertexIndex v_index[3];
		v_index[0] = face_fan_slice.first;
		v_index[1] = face_fan_slice.second.first;
		v_index[2] = face_fan_slice.second.second;

		Eigen::Vector2d v[3];

		for(int i = 0; i < 3; i++)
		{
			const RDS::SparseVariableIndex v_x_index = mesh_data_provider_->GetXVariableIndex(v_index[i]);
			const RDS::SparseVariableIndex v_y_index = mesh_data_provider_->GetYVariableIndex(v_index[i]);

			const double v_x = x.coeffRef(v_x_index);
			const double v_y = x.coeffRef(v_y_index);

			variables_.coeffRef(v_x_index) = v_x;
			variables_.coeffRef(v_y_index) = v_y;

			v[i].coeffRef(0) = v_x;
			v[i].coeffRef(1) = v_y;
		}

		Eigen::Vector2d e1 = v[1] - v[0];
		Eigen::Vector2d e2 = v[2] - v[0];

		e1.normalize();
		e2.normalize();

		const double current_angle = abs(acos(e1.dot(e2)));
		accumulated_angle += current_angle;
	}

	angle_ = accumulated_angle;
}

void FaceFanDataProvider::Update(const Eigen::VectorXd& x, int32_t update_modifiers)
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