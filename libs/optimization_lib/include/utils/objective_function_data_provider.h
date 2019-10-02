#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_DATA_PROVIDER_H

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class ObjectiveFunctionDataProvider
{
public:
	using ObjectiveFunctionDataProviderEventHandler = std::function<void(std::string)>;

	virtual const Eigen::MatrixX3i& GetDomainFaces() const = 0;
	virtual const Eigen::MatrixX3d& GetDomainVertices() const = 0;
	virtual const Eigen::MatrixX2i& GetDomainEdges() const = 0;
	virtual const Eigen::MatrixX3i& GetImageFaces() const = 0;
	virtual const Eigen::DenseIndex GetImageVerticesCount() const = 0;
	virtual const Eigen::MatrixX3d& GetD1() const = 0;
	virtual const Eigen::MatrixX3d& GetD2() const = 0;

	// Relevant for objective functions that operate on triangle soups
	virtual const Eigen::SparseMatrix<double>& GetCorrespondingVertexPairsCoefficients() const = 0;
	virtual const Eigen::VectorXd& GetCorrespondingVertexPairsEdgeLength() const = 0;

	// Event handling
	void Subscribe(const std::string& event_name, const ObjectiveFunctionDataProviderEventHandler& event_handler);

protected:
	void Publish(const std::string& event_name);

	std::unordered_map<std::string, std::vector<ObjectiveFunctionDataProviderEventHandler>> event_handlers;
};

#endif