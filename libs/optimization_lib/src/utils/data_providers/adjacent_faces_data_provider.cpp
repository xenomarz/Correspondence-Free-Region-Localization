#include <utils/data_providers/adjacent_faces_data_provider.h>

AdjacentFacesDataProvider::AdjacentFacesDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::AdjacentFacesVertices& adjacent_faces_vertices) :
	DataProvider(mesh_data_provider),
	adjacent_faces_vertices_(adjacent_faces_vertices)
{

}

AdjacentFacesDataProvider::~AdjacentFacesDataProvider()
{

}

void AdjacentFacesDataProvider::Update(const Eigen::VectorXd& x)
{
	double accumulated_angle = 0;
	const auto adjacent_faces_count = adjacent_faces_vertices_.size();
	for (std::size_t adjacent_face_index = 0; adjacent_face_index < adjacent_faces_count; adjacent_face_index++)
	{
		RDS::AdjacentFaceVertices adjacent_face_vertices = adjacent_faces_vertices_[adjacent_face_index];
		
		RDS::VertexIndex v0_index = adjacent_face_vertices.first;
		RDS::VertexIndex v1_index = adjacent_face_vertices.second.first;
		RDS::VertexIndex v2_index = adjacent_face_vertices.second.second;

		Eigen::Vector2d v0;
		Eigen::Vector2d v1;
		Eigen::Vector2d v2;

		v0.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetVertexXIndex(v0_index));
		v0.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetVertexYIndex(v0_index));

		v1.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetVertexXIndex(v1_index));
		v1.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetVertexYIndex(v1_index));

		v2.coeffRef(0) = x.coeffRef(mesh_data_provider_->GetVertexXIndex(v2_index));
		v2.coeffRef(1) = x.coeffRef(mesh_data_provider_->GetVertexYIndex(v2_index));

		Eigen::Vector2d e1 = v1 - v0;
		Eigen::Vector2d e2 = v2 - v0;

		e1.normalize();
		e2.normalize();

		const double current_angle = abs(acos(e1.dot(e2)));
		accumulated_angle += current_angle;
	}

	angle_ = accumulated_angle;
}

double AdjacentFacesDataProvider::GetAngle() const
{
	return angle_;
}