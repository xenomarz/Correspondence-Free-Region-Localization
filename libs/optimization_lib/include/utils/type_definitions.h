#pragma once
#ifndef OPTIMIZATION_LIB_TYPE_DEFINITIONS_H
#define OPTIMIZATION_LIB_TYPE_DEFINITIONS_H

namespace RDS
{
	/**
	 * Index types
	 */
	using VertexIndex = std::int64_t;
	using EdgeIndex = std::int64_t;
	using FaceIndex = std::int64_t;
	using EdgeDescriptor = std::pair<VertexIndex, VertexIndex>;
	using EdgePairDescriptor = std::pair<EdgeDescriptor, EdgeDescriptor>;
	using SparseVariableIndex = std::int64_t;
	using DenseVariableIndex = std::int64_t;
	using HessianTripletIndex = std::int64_t;
	using PartialDerivativeIndex = std::int64_t;
	using HessianEntry = std::pair<DenseVariableIndex, DenseVariableIndex>;
}

#endif