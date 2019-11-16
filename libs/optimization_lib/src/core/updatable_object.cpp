// Optimization lib includes
#include <core/updatable_object.h>

UpdatableObject::UpdatableObject()
{
	
}

UpdatableObject::~UpdatableObject()
{
	
}

void UpdatableObject::Update(const Eigen::VectorXd& x, UpdatedObjectSet& updated_objects)
{
	if(ShouldUpdate(updated_objects))
	{
		Update(x);
	}
}

bool UpdatableObject::ShouldUpdate(UpdatedObjectSet& updated_objects)
{
	if (updated_objects.find(this) == updated_objects.end())
	{
		updated_objects.insert(this);
		return true;
	}

	return false;
}