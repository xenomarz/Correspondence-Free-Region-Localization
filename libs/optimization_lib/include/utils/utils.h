#pragma once
#ifndef OPTIMIZATION_LIB_UTILS_H
#define OPTIMIZATION_LIB_UTILS_H

// STL includes
#include <limits>

// Boost includes
#include <boost/functional/hash.hpp>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "../objective_functions/objective_function.h"

class Utils
{
public:
	/**
	 * SVD
	 */
	static inline void SSVD2x2(const Eigen::Matrix2d& A, Eigen::Matrix2d& U, Eigen::Matrix2d& S, Eigen::Matrix2d& V)
	{
		double e = (A(0) + A(3)) * 0.5;
		double f = (A(0) - A(3)) * 0.5;
		double g = (A(1) + A(2)) * 0.5;
		double h = (A(1) - A(2)) * 0.5;
		double q = sqrt((e * e) + (h * h));
		double r = sqrt((f * f) + (g * g));
		double a1 = atan2(g, f);
		double a2 = atan2(h, e);
		double rho = (a2 - a1) * 0.5;
		double phi = (a2 + a1) * 0.5;

		S(0) = q + r;
		S(1) = 0;
		S(2) = 0;
		S(3) = q - r;

		double c = cos(phi);
		double s = sin(phi);
		U(0) = c;
		U(1) = s;
		U(2) = -s;
		U(3) = c;

		c = cos(rho);
		s = sin(rho);
		V(0) = c;
		V(1) = -s;
		V(2) = s;
		V(3) = c;
	}

	/**
	 * Remove row & column from matrix
	 */
	
	// https://stackoverflow.com/questions/13290395/how-to-remove-a-certain-row-or-column-while-using-eigen-library-c
	template<typename MatrixType>
	static inline void RemoveRow(MatrixType& x, unsigned int row_to_remove)
	{
		unsigned int rows = x.rows() - 1;
		unsigned int cols = x.cols();

		if (row_to_remove < rows)
		{
			x.block(row_to_remove, 0, rows - row_to_remove, cols) = x.bottomRows(rows - row_to_remove);
		}

		x.conservativeResize(rows, cols);
	}

	// https://stackoverflow.com/questions/13290395/how-to-remove-a-certain-row-or-column-while-using-eigen-library-c
	template<typename MatrixType>
	static inline void RemoveColumn(MatrixType& x, unsigned int column_to_remove)
	{
		auto rows = x.rows();
		auto cols = x.cols() - 1;

		if (column_to_remove < cols)
		{
			x.block(0, column_to_remove, rows, cols - column_to_remove) = x.rightCols(cols - column_to_remove);
		}

		x.conservativeResize(rows, cols);
	}

	/**
	 * Barycenter calculation
	 */
	template <typename MatrixType, typename VectorType>
	static inline bool CalculateBarycenter(const std::vector<int64_t>& indices, const Eigen::Map<MatrixType>& X, VectorType& barycenter)
	{
		const auto indices_count = indices.size();
		if(indices_count > 0)
		{
			barycenter = VectorType::Zero(X.cols());
			for (int32_t i = 0; i < indices_count; i++)
			{
				barycenter += X.row(indices[i]);
			}

			barycenter /= indices_count;

			return true;
		}

		return false;
	}

	template <typename Derived, typename VectorType>
	static inline bool CalculateBarycenter(const std::vector<int64_t>& indices, const Eigen::MatrixBase<Derived>& X, VectorType& barycenter)
	{
		const auto indices_count = indices.size();
		if (indices_count > 0)
		{
			barycenter = VectorType::Zero(X.cols());
			for (int32_t i = 0; i < indices_count; i++)
			{
				barycenter += X.row(indices[i]);
			}

			barycenter /= indices_count;

			return true;
		}

		return false;
	}

	template <typename MatrixType, typename VectorType>
	static inline bool CalculateBarycenter(const Eigen::VectorXi& indices, const Eigen::Map<MatrixType>& X, VectorType& barycenter)
	{
		auto indices_internal = std::vector<int64_t>(indices.data(), indices.data() + indices.rows());
		return CalculateBarycenter(indices_internal, X, barycenter);
	}

	template <typename Derived, typename VectorType>
	static inline bool CalculateBarycenter(const Eigen::VectorXi& indices, const Eigen::MatrixBase<Derived>& X, VectorType& barycenter)
	{
		auto indices_internal = std::vector<int64_t>(indices.data(), indices.data() + indices.rows());
		return CalculateBarycenter(indices_internal, X, barycenter);
	}

	template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
	static Eigen::VectorXd GetApproximatedGradient(const std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>>& objective_function, const Eigen::VectorXd& x)
	{
		Eigen::VectorXd g(x.rows());
		g.setZero();
		
		Eigen::VectorXd perturbation(x.rows());

		const double epsilon = CalculateEpsilon(x);
		const double epsilon2 = 2 * epsilon;
		
		for(int64_t i = 0; i < x.rows(); i++)
		{
			perturbation.setZero();
			perturbation.coeffRef(i) = epsilon;
			Eigen::VectorXd x_plus_eps = x + perturbation;
			Eigen::VectorXd x_minus_eps = x - perturbation;

			objective_function->Update(x_plus_eps);
			const double value_plus = objective_function->GetValue();

			objective_function->Update(x_minus_eps);
			const double value_minus = objective_function->GetValue();

			g.coeffRef(i) = (value_plus - value_minus) / epsilon2;
		}

		return g;
	}

	template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
	static Eigen::MatrixXd GetApproximatedHessian(const std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>>& objective_function, const Eigen::VectorXd& x)
	{
		Eigen::MatrixXd H(x.rows(), x.rows());
		H.setZero();
		
		Eigen::VectorXd perturbation(x.rows());

		const double epsilon = CalculateEpsilon(x);
		const double epsilon2 = 2 * epsilon;
		for (int64_t i = 0; i < x.rows(); i++)
		{
			perturbation.setZero();
			perturbation.coeffRef(i) = epsilon;
			Eigen::VectorXd x_plus_eps = x + perturbation;
			Eigen::VectorXd x_minus_eps = x - perturbation;

			objective_function->Update(x_plus_eps);
			const Eigen::VectorXd g_plus = objective_function->GetGradient();

			objective_function->Update(x_minus_eps);
			const Eigen::VectorXd g_minus = objective_function->GetGradient();

			H.row(i) = (g_plus - g_minus) / epsilon2;
		}

		return H;
	}

private:
	static double CalculateEpsilon(const Eigen::VectorXd& x)
	{
		const double machine_epsilon = std::numeric_limits<double>::epsilon();
		const double max = x.cwiseAbs().maxCoeff();
		return std::cbrt(machine_epsilon) * max;
	}
};

#endif