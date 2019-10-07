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
	 * Public type definitions
	 */
	enum class OffsetType {
		RELATIVE_TO_INITIAL,
		RELATIVE_TO_CURRENT
	};


	/**
	 * Constructors and destructor
	 */
	Position(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider);
	virtual ~Position();

	/**
	 * Public Methods
	 */
	void AddConstrainedVertex(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position);
	void AddConstrainedVertices(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_position_pairs);
	void UpdateConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_position);
	void OffsetConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_offset, const OffsetType offset_type = OffsetType::RELATIVE_TO_INITIAL);
	void OffsetConstrainedVerticesPositions(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_offset_pairs, const OffsetType offset_type = OffsetType::RELATIVE_TO_INITIAL);
	Eigen::Vector2d GetConstrainedVertexPosition(const Eigen::DenseIndex vertex_index);
	void RemoveConstrainedVertex(const Eigen::DenseIndex vertex_index);
	void RemoveConstrainedVertices(const std::vector<Eigen::DenseIndex>& vertices_indices);
	bool IsValid();

private:
	/**
	 * Private type definitions
	 */
	using ConstrainedVertex = std::pair<Eigen::DenseIndex, Eigen::Vector2d>;

	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::VectorXd& x, double& f);
	void CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss);
	void PreUpdate(const Eigen::VectorXd& x);

	/**
	 * Fields
	 */
	std::unordered_map<Eigen::DenseIndex, Eigen::DenseIndex> im_vi_2_ci_;
	Eigen::DenseIndex constrained_vertices_count_;
	Eigen::MatrixX2d x_constrained_initial_;
	Eigen::MatrixX2d x_constrained_;
	Eigen::MatrixX2d x_current_;
	Eigen::MatrixX2d x_diff_;
};

#endif