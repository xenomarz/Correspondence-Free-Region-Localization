#pragma once
#ifndef OPTIMIZATION_LIB_POSITION
#define OPTIMIZATION_LIB_POSITION

// STL includes
#include <vector>
#include <mutex>
#include <utility>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./objective_function.h"

class Position : public ObjectiveFunction
{
public:
	void AddConstrainedVertex(Eigen::DenseIndex vertex_index, Eigen::Vector2d vertex_position);
	void RemoveConstrainedVertex(Eigen::DenseIndex vertex_index);

private:

	/**
	 * Private type definitions
	 */

	using ConstrainedVertex = std::pair<Eigen::DenseIndex, Eigen::Vector2d>;

	/**
	 * Constructor and destructor
	 */
	
	Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);
	virtual ~Position();

	/**
	 * Overrides
	 */

	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& x, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& x, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& x, std::vector<double>& ss);
	void PreUpdate(const Eigen::MatrixX2d& x);

	/**
	 * Methods
	 */

	void UpdateConstrainedPositionsMatrix();

	/**
	 * Fields
	 */

	std::vector<ConstrainedVertex> constrained_vertices_;
	Eigen::DenseIndex constrained_vertices_count_;
	Eigen::MatrixX2d x_constrained_;
	Eigen::MatrixX2d x_current_;
	Eigen::MatrixX2d x_diff_;
};

#endif