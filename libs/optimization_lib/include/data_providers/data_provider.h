#pragma once
#ifndef OPTIMIZATION_LIB_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_DATA_PROVIDER_H

// Optimization Lib Includes
#include "../core/updatable_object.h"
#include "./mesh_data_provider.h"

class DataProvider : public UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	enum class CoordinateType : int32_t
	{
		X,
		Y
	};
	
	/**
	 * Constructors and destructor
	 */
	DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~DataProvider();

	/**
	 * Getters
	 */
	const Eigen::SparseVector<double>& GetVariables() const;
	double GetVariableValue(const RDS::SparseVariableIndex variable_index) const;
	double GetVariableValue(const RDS::VertexIndex vertex_index, const CoordinateType coordinate_type) const;
	
protected:
	Eigen::SparseVector<double> variables_;
};

#endif