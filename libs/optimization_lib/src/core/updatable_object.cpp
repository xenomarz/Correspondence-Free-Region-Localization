// Optimization Lib Includes
#include "./core/updatable_object.h"

UpdatableObject::UpdatableObject()
{
	
}

UpdatableObject::~UpdatableObject()
{
	
}

void UpdatableObject::Update(const Eigen::VectorXd& x, std::unordered_set<UpdatableObject*>& updated_objects)
{
	if(!updated_objects.contains(this))
	{
		Update(x);
		updated_objects.insert(this);
	}
}