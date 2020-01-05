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
	virtual int64_t GetImageEdgesCount() const = 0;
	virtual int64_t GetDomainEdgesCount() const = 0;
	virtual const Eigen::MatrixX3d& GetD1() const = 0;
	virtual const Eigen::MatrixX3d& GetD2() const = 0;
	virtual RDS::SparseVariableIndex GetXVariableIndex(RDS::VertexIndex vertex_index) const = 0;
	virtual RDS::SparseVariableIndex GetYVariableIndex(RDS::VertexIndex vertex_index) const = 0;
	virtual RDS::VertexIndex GetVertexIndex(RDS::SparseVariableIndex variable_index) const = 0;
	virtual RDS::VertexIndex GetDomainVertexIndex(RDS::VertexIndex image_vertex_index) const = 0;
	virtual int64_t GetVariablesCount() const = 0;
	virtual const RDS::FaceFans& GetFaceFans() const = 0;
	virtual RDS::EdgeIndex GetDomainEdgeIndex(RDS::EdgeDescriptor image_edge_descriptor) const = 0;
	virtual RDS::EdgeIndex GetImageEdgeIndex(RDS::EdgeDescriptor image_edge_descriptor) const = 0;
	
	// Relevant for objective functions that operate on triangle soups
	virtual const Eigen::SparseMatrix<double>& GetCorrespondingVertexPairsCoefficients() const = 0;
	virtual const Eigen::VectorXd& GetCorrespondingVertexPairsEdgeLength() const = 0;
};

#endif