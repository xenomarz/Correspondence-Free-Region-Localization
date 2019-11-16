#pragma once
#ifndef OPTIMIZATION_LIB_UPDATABLE_OBJECT_H
#define OPTIMIZATION_LIB_UPDATABLE_OBJECT_H

// Eigen Includes
#include <Eigen/Core>

// TBB includes
#include <tbb/concurrent_unordered_set.h>
//#include <unordered_set>

class UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	using UpdatedObjectSet = tbb::concurrent_unordered_set<UpdatableObject*>;
	
	/**
	 * Constructors and destructor
	 */
	UpdatableObject();
	virtual ~UpdatableObject();

	/**
	 * Public methods
	 */
	virtual void Update(const Eigen::VectorXd& x) = 0;
	virtual void Update(const Eigen::VectorXd& x, UpdatedObjectSet& updated_objects);

protected:
	/**
	 * Protected methods
	 */
	bool ShouldUpdate(UpdatedObjectSet& updated_objects);
};

#endif