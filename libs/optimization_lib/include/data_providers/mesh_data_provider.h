#pragma once
#ifndef OPTIMIZATION_LIB_MESH_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_MESH_DATA_PROVIDER_H

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "../core/core.h"

class MeshDataProvider
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

	RDS::SparseVariableIndex GetVertexXIndex(RDS::VertexIndex vertex_index) const
	{
		return vertex_index;
	}

	RDS::SparseVariableIndex GetVertexYIndex(RDS::VertexIndex vertex_index) const
	{
		return vertex_index + GetImageVerticesCount();
	}

	int64_t GetVariablesCount() const
	{
		return 	2 * GetImageVerticesCount();
	}

	// Relevant for objective functions that operate on triangle soups
	virtual const Eigen::SparseMatrix<double>& GetCorrespondingVertexPairsCoefficients() const = 0;
	virtual const Eigen::VectorXd& GetCorrespondingVertexPairsEdgeLength() const = 0;
};

#endif