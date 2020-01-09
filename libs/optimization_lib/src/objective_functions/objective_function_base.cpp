// Optimization lib includes
#include <core/updatable_object.h>
#include <objective_functions/objective_function_base.h>

ObjectiveFunctionBase::ObjectiveFunctionBase(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
	UpdatableObject(mesh_data_provider)
{
	
}

ObjectiveFunctionBase::~ObjectiveFunctionBase()
{
	
}

ObjectiveFunctionBase::UpdateOptions operator | (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs)
{
	using T = std::underlying_type_t<ObjectiveFunctionBase::UpdateOptions>;
	return static_cast<ObjectiveFunctionBase::UpdateOptions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

ObjectiveFunctionBase::UpdateOptions& operator |= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

ObjectiveFunctionBase::UpdateOptions operator & (const ObjectiveFunctionBase::UpdateOptions lhs, const ObjectiveFunctionBase::UpdateOptions rhs)
{
	using T = std::underlying_type_t<ObjectiveFunctionBase::UpdateOptions>;
	return static_cast<ObjectiveFunctionBase::UpdateOptions>(static_cast<T>(lhs)& static_cast<T>(rhs));
}

ObjectiveFunctionBase::UpdateOptions& operator &= (ObjectiveFunctionBase::UpdateOptions& lhs, const ObjectiveFunctionBase::UpdateOptions rhs)
{
	lhs = lhs & rhs;
	return lhs;
}