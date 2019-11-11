#include "./utils/data_providers/image_data_provider.h"

ImageDataProvider::ImageDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	DataProvider(mesh_data_provider)
{
	
}

ImageDataProvider::ImageDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const Eigen::VectorXd& x0) :
	ImageDataProvider(mesh_data_provider)
{
	Update(x0);
}

ImageDataProvider::~ImageDataProvider()
{
	
}

void ImageDataProvider::Update(const Eigen::VectorXd& x)
{
	x_ = x;
	for(std::size_t i = 0; i < data_providers_.size(); i++)
	{
		data_providers_.at(i)->Update(x);
	}
}

void ImageDataProvider::AddEdgePairDescriptors(const std::vector<RDS::EdgePairDescriptor>& edge_pair_descriptors)
{
	for(std::size_t i = 0; i < edge_pair_descriptors.size(); i++)
	{
		auto edge_pair_descriptor = edge_pair_descriptors.at(i);
		auto edge_pair_data_provider = std::make_shared<EdgePairDataProvider>(mesh_data_provider_, edge_pair_descriptor);
		edge_pair_descriptor_to_edge_pair_data_provider_map_.insert({ edge_pair_descriptor, edge_pair_data_provider });
		data_providers_.push_back(edge_pair_data_provider);
	}
}

std::shared_ptr<EdgePairDataProvider> ImageDataProvider::GetEdgePairDataProvider(const RDS::EdgePairDescriptor& edge_pair_descriptor)
{
	return edge_pair_descriptor_to_edge_pair_data_provider_map_[edge_pair_descriptor];
}

const Eigen::VectorXd& ImageDataProvider::GetX() const
{
	return x_;
}