// STL includes
#include <algorithm>

// Optimization lib includes
#include <core/updatable_object.h>

UpdatableObject::UpdatableObject(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	mesh_data_provider_(mesh_data_provider)
{
	
}

UpdatableObject::~UpdatableObject()
{
	
}

void UpdatableObject::Initialize()
{
	InitializeDependencyLayers(dependency_layers_);
}

[[nodiscard]] std::shared_ptr<MeshDataProvider> UpdatableObject::GetMeshDataProvider() const
{
	return mesh_data_provider_;
}

const tbb::concurrent_vector<std::shared_ptr<UpdatableObject>>& UpdatableObject::GetDependencies() const
{
	return dependencies_;
}

void UpdatableObject::InitializeDependencyLayers(std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers)
{
	dependency_layers_.clear();
	for (const auto& dependency : dependencies_)
	{
		BuildDependencyLayers(dependency, dependency_layers);
	}
}

int UpdatableObject::BuildDependencyLayers(const std::shared_ptr<UpdatableObject>& updatable_object, std::vector<std::vector<std::shared_ptr<UpdatableObject>>>& dependency_layers) const
{
	if(updatable_object->GetDependencies().empty())
	{
		if(dependency_layers.empty())
		{
			dependency_layers.resize(1);
		}

		dependency_layers[0].push_back(updatable_object);
		return 0;
	}

	std::vector<int> heights;
	for (const auto& dependency : updatable_object->GetDependencies())
	{
		heights.push_back(BuildDependencyLayers(dependency, dependency_layers));
	}

	const auto layer_index = (*std::max_element(heights.begin(), heights.end())) + 1;
	const auto minimal_layers_count = layer_index + 1;
	if (dependency_layers.size() < minimal_layers_count)
	{
		dependency_layers.resize(minimal_layers_count);
	}
	
	dependency_layers[layer_index].push_back(updatable_object);
	return layer_index;
}