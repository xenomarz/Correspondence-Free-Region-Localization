#pragma once
#ifndef OPTIMIZATION_LIB_POSITION_H
#define OPTIMIZATION_LIB_POSITION_H

// STL includes
#include <vector>
#include <mutex>
#include <utility>
#include <functional>
#include <map>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./objective_function.h"

class Position : public ObjectiveFunction
{
public:

	/**
	 * Constructors and destructor
	 */
	Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);
	virtual ~Position();

	/**
	 * Public Methods
	 */
	void AddConstrainedVertex(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position);
	void ResetConstrainedVertexPosition(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position);
	void OffsetConstrainedVertexPosition(Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_offset);
	Eigen::Vector2d GetConstrainedVertexPosition(Eigen::DenseIndex vertex_index);
	void RemoveConstrainedVertex(Eigen::DenseIndex vertex_index);

private:

	/**
	 * Private type definitions
	 */
	using ConstrainedVertex = std::pair<Eigen::DenseIndex, Eigen::Vector2d>;

	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& x, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& x, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& x, std::vector<double>& ss);
	void PreUpdate(const Eigen::MatrixX2d& x);

	/**
	 * Fields
	 */
	std::unordered_map<Eigen::DenseIndex, Eigen::DenseIndex> im_vi_2_ci_;
	Eigen::DenseIndex constrained_vertices_count_;
	Eigen::MatrixX2d x_constrained_;
	Eigen::MatrixX2d x_current_;
	Eigen::MatrixX2d x_diff_;
};

#endif