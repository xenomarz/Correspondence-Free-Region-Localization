#pragma once
#ifndef OPTIMIZATION_LIB_UPDATABLE_OBJECT_H
#define OPTIMIZATION_LIB_UPDATABLE_OBJECT_H

// STL includes
#include <unordered_set>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "../utils/type_definitions.h"

class UpdatableObject
{
public:
	/**
	 * Constructors and destructor
	 */
	UpdatableObject();
	~UpdatableObject();

	/**
	 * Public methods
	 */
	virtual void Update(const Eigen::VectorXd& x) = 0;
	void Update(const Eigen::VectorXd& x, std::unordered_set<UpdatableObject*>& updated_objects);
};

#endif