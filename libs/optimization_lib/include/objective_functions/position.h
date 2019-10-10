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
	void UpdateConstrainedVerticesPositions(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_position_pairs);
	void OffsetConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, const Eigen::Vector2d& vertex_offset, const OffsetType offset_type = OffsetType::RELATIVE_TO_INITIAL);
	void OffsetConstrainedVerticesPositions(const std::vector<std::pair<Eigen::DenseIndex, Eigen::Vector2d>>& index_offset_pairs, const OffsetType offset_type = OffsetType::RELATIVE_TO_INITIAL);
	bool GetConstrainedVertexPosition(const Eigen::DenseIndex vertex_index, Eigen::Vector2d& vertex_position);
	void RemoveConstrainedVertex(const Eigen::DenseIndex vertex_index);
	void RemoveConstrainedVertices(const std::vector<Eigen::DenseIndex>& vertices_indices);
	void ResetConstrainedVertex(const Eigen::DenseIndex vertex_index);
	void ResetConstrainedVertices(const std::vector<Eigen::DenseIndex>& vertices_indices);

private:
	/**
	 * Private type definitions
	 */
	class ConstrainedVertexData
	{
	public:
		ConstrainedVertexData(const uint32_t internal_index, const uint32_t external_index, const Eigen::Vector2d& initial_position);

		void SetInternalIndex(const uint32_t internal_index);
		void SetInitialPosition(const Eigen::Vector2d& position);
		void SetCurrentPosition(const Eigen::Vector2d& position);
		const uint32_t GetInternalIndex() const;
		const uint32_t GetExternalIndex() const;
		const Eigen::Vector2d& GetInitialPosition() const;
		const Eigen::Vector2d& GetCurrentPosition() const;
		
	private:
		uint32_t internal_index_;
		uint32_t external_index_;
		Eigen::Vector2d current_position_;
		Eigen::Vector2d initial_position_;
	};
	
	using ConstrainedVertex = std::pair<Eigen::DenseIndex, Eigen::Vector2d>;

	/**
	 * Overrides
	 */
	void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) override;
	void CalculateValue(const Eigen::VectorXd& x, double& f, Eigen::VectorXd& f_per_vertex) override;
	void CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g) override;
	void CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss) override;
	void PreUpdate(const Eigen::VectorXd& x) override;

	/***
	 * Private methods
	 */
	void ResetConstraintsMatrices();

	/**
	 * Fields
	 */
	std::unordered_map<Eigen::DenseIndex, std::unique_ptr<ConstrainedVertexData>> ei_2_cv_data;
	Eigen::MatrixX2d x_constrained_initial_;
	Eigen::MatrixX2d x_constrained_;
	Eigen::MatrixX2d x_current_;
	Eigen::MatrixX2d x_diff_;
};

#endif