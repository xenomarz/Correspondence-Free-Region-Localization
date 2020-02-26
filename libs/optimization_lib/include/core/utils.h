#pragma once
#ifndef OPTIMIZATION_LIB_UTILS_H
#define OPTIMIZATION_LIB_UTILS_H

// Boost includes
#include <boost/functional/hash.hpp>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

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
	 * Remove row & column from eigen matrix
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
	static Eigen::VectorXd CalculateBarycenter(const std::vector<int64_t>& indices, const Eigen::VectorXd& x)
	{
		const auto indices_count = indices.size();
		if(indices_count > 0)
		{
			auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
			auto barycenter = Eigen::VectorXd::Zero(X.cols());
			for (int32_t i = 0; i < indices_count; i++)
			{
				barycenter += X.row(indices[i]);
			}

			barycenter /= indices_count;

			return barycenter;
		}

		throw std::exception("Empty indices vector");
	}

	/**
	 * Hash generation methods
	 */
	template <class Derived>
	std::size_t GenerateHash(const Eigen::MatrixBase<Derived>& matrix_base) const
	{
		size_t seed = 0;
		for (size_t i = 0; i < matrix_base.size(); ++i) {
			auto value = *(matrix_base.data() + i);
			boost::hash_combine(seed, value);
		}
		return seed;
	}
};

#endif