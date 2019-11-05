#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_DATA_PROVIDER_H

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class ObjectiveFunctionDataProvider
{
public:
	virtual const Eigen::MatrixX3i& GetDomainFaces() const = 0;
	virtual const Eigen::MatrixX3d& GetDomainVertices() const = 0;
	virtual const Eigen::MatrixX2i& GetDomainEdges() const = 0;
	virtual const Eigen::MatrixX3i& GetImageFaces() const = 0;
	virtual int64_t GetImageVerticesCount() const = 0;
	virtual int64_t GetDomainVerticesCount() const = 0;
	virtual const Eigen::MatrixX3d& GetD1() const = 0;
	virtual const Eigen::MatrixX3d& GetD2() const = 0;

	// Relevant for objective functions that operate on triangle soups
	virtual const Eigen::SparseMatrix<double>& GetCorrespondingVertexPairsCoefficients() const = 0;
	virtual const Eigen::VectorXd& GetCorrespondingVertexPairsEdgeLength() const = 0;
};

#endif