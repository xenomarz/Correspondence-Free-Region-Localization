// Optimization lib includes
#include <core/updatable_object.h>

UpdatableObject::UpdatableObject(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	mesh_data_provider_(mesh_data_provider)
{
	
}

UpdatableObject::~UpdatableObject()
{
	
}

[[nodiscard]] std::shared_ptr<MeshDataProvider> UpdatableObject::GetMeshDataProvider() const
{
	return mesh_data_provider_;
}