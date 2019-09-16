#pragma once
#ifndef OPTIMIZATION_LIB_POSITION
#define OPTIMIZATION_LIB_POSITION

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include <objective_functions/objective_function.h>

class Position : public ObjectiveFunction
{
public:
	void AddConstrainedVertex(Eigen::DenseIndex vertex_index);
	void RemoveConstrainedVertex(Eigen::DenseIndex vertex_index);

private:
	Position(const std::shared_ptr<MeshWrapper>& mesh_wrapper);
	virtual ~Position();

	/**
	 * Overrides
	 */
	void InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss);
	void CalculateValue(const Eigen::MatrixX2d& X, double& f);
	void CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g);
	void CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss);

	/**
	 * Fields
	 */
	std::vector<Eigen::DenseIndex> constrained_vertices_indices;
	Eigen::MatrixX2d X_constained;
};

#endif