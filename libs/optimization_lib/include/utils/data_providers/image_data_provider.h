#pragma once
#ifndef OPTIMIZATION_LIB_IMAGE_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_IMAGE_DATA_PROVIDER_H

// STL includes
#include <vector>
#include <memory>

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./utils/type_definitions.h"
#include "./utils/data_providers/data_provider.h"
#include "./utils/data_providers/edge_pair_data_provider.h"

class ImageDataProvider : public DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	ImageDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	ImageDataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const Eigen::VectorXd& x0);
	virtual ~ImageDataProvider();

	/**
	 * Public methods
	 */
	void Update(const Eigen::VectorXd& x) override;
	void AddEdgePairDescriptors(const std::vector<RDS::EdgePairDescriptor>& edge_pair_descriptors);
	std::shared_ptr<EdgePairDataProvider> GetEdgePairDataProvider(const RDS::EdgePairDescriptor& edge_pair_descriptor);
	const Eigen::VectorXd& GetX() const;
	
private:
	/**
	 * Private type definitions
	 */
	using EdgePairDescriptorToEdgePairDataProviderMap = std::unordered_map<RDS::EdgePairDescriptor, std::shared_ptr<EdgePairDataProvider>, RDS::EdgePairDescriptorHash, RDS::EdgePairDescriptorEquals>;

	/**
	 * Private fields
	 */
	Eigen::VectorXd x_;
	std::vector<std::shared_ptr<DataProvider>> data_providers_;
	EdgePairDescriptorToEdgePairDataProviderMap edge_pair_descriptor_to_edge_pair_data_provider_map_;
};

#endif