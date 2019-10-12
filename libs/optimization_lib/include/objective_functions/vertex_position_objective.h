#pragma once
#ifndef OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./position_objective.h"

class VertexPositionObjective : public PositionObjective
{
public:
	/**
	 * Constructors and destructor
	 */
	VertexPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::pair<int64_t, Eigen::Vector2d>>& index_vertex_pairs);
	virtual ~VertexPositionObjective();

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
	std::vector<std::pair<int64_t, Eigen::Vector2d>> index_vertex_pairs_;
	Eigen::MatrixX2d X_objective_;
	Eigen::MatrixX2d X_current_;
	Eigen::MatrixX2d X_diff_;
};

#endif