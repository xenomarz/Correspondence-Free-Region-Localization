#include <utils/data_providers/face_fan_data_provider.h>

FaceFanDataProvider::FaceFanDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const RDS::VertexIndex vertex_index, const RDS::VertexAdjacentFacesVertices& vertex_adjacent_faces_vertices) :
	DataProvider(mesh_data_provider),
	vertex_index_(vertex_index),
	vertex_adjacent_faces_vertices_(vertex_adjacent_faces_vertices)
{

}

FaceFanDataProvider::~FaceFanDataProvider()
{

}

void FaceFanDataProvider::Update(const Eigen::VectorXd& x)
{
	double accumulated_angle = 0;
	const auto vertex_adjacent_faces_vertices_count = vertex_adjacent_faces_vertices_.size();
	for (std::size_t adjacent_face_index = 0; adjacent_face_index < vertex_adjacent_faces_vertices_count; adjacent_face_index++)
	{
		RDS::VertexAdjacentFaceVertices vertex_adjacent_face_vertices = vertex_adjacent_faces_vertices_[adjacent_face_index];
		
		int64_t v0_index = vertex_index_;
		int64_t v1_index = vertex_adjacent_face_vertices.first;
		int64_t v2_index = vertex_adjacent_face_vertices.second;

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

double FaceFanDataProvider::GetAngle() const
{
	return angle_;
}