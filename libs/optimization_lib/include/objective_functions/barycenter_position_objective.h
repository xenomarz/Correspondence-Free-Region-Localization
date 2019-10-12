#pragma once
#ifndef OPTIMIZATION_LIB_BARYCENTER_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_BARYCENTER_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./position_objective.h"

class BarycenterPositionObjective : public PositionObjective
{
public:
	/**
	 * Constructors and destructor
	 */
	BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<int64_t>& indices, const Eigen::Vector2d& objective_barycenter);
	BarycenterPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const Eigen::VectorXi& indices, const Eigen::Vector2d& objective_barycenter);
	virtual ~BarycenterPositionObjective();

private:
	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) override;
	void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) override;
	void CalculateGradient(Eigen::VectorXd& g) override;
	void CalculateHessian(std::vector<double>& ss) override;
	void PreUpdate(const Eigen::VectorXd& x) override;
	void OffsetPositionConstraint(const Eigen::Vector2d& offset) override;

	/**
	 * Fields
	 */
	std::vector<int64_t> indices_;
	Eigen::Vector2d objective_barycenter_;
	Eigen::Vector2d current_barycenter_;
	Eigen::Vector2d barycenters_diff_;
};

#endif