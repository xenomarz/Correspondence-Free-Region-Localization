#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_BASE_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_BASE_H

// STL includes
#include <any>

// Eigen Includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/mesh_data_provider.h"

class ObjectiveFunctionBase : public UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	enum class UpdateOptions : int32_t
	{
		None = 0,
		Value = 1,
		ValuePerVertex = 2,
		Gradient = 4,
		Hessian = 8,
		All = 15
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
	 * Getters
	 */
	virtual bool GetProperty(const int32_t property_id, const int32_t property_modifier_id, std::any& property_value) = 0;

	/**
	 * Setters
	 */
	virtual bool SetProperty(const int32_t property_id, const std::any& property_value) = 0;
};

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
ObjectiveFunctionBase::UpdateOptions operator | (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions& operator |= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions operator & (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
ObjectiveFunctionBase::UpdateOptions& operator &= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs);
#endif