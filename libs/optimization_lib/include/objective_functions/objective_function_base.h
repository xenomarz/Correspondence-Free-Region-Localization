#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_BASE_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_BASE_H

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/mesh_data_provider.h"

//#include <unordered_set>

class ObjectiveFunctionBase : public UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	enum class UpdateOptions : int32_t
	{
		NONE = 0,
		VALUE = 1,
		VALUE_PER_VERTEX = 2,
		GRADIENT = 4,
		HESSIAN = 8,
		ALL = 15
	};

	enum class Properties : int32_t
	{
		Value,
		ValuePerVertex,
		Gradient,
		GradientNorm,
		Hessian,
		Weight,
		Name,
		Count_
	};

	enum class PropertyModifiers : int32_t
	{
		None,
		Domain,
		Image,
		Count_
	};

	/**
	 * Constructors and destructor
	 */
	ObjectiveFunctionBase(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~ObjectiveFunctionBase();

	/**
	 * Public methods
	 */
	virtual void Update(const Eigen::VectorXd& x, const UpdateOptions update_options) = 0;
};

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
ObjectiveFunctionBase::UpdateOptions operator | (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions& operator |= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions operator & (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions& operator &= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs);

#endif