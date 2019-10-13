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
	 * Custom hash and equals function objects for unordered_map
	 */
	
	// https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
	struct PairHash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& pair) const
		{
			auto minmax_pair = std::minmax(pair.first, pair.second);
			std::size_t seed = 0;

			// https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values/35991300#35991300
			boost::hash_combine(seed, minmax_pair.first);
			boost::hash_combine(seed, minmax_pair.second);
			return seed;
		}
	};

	struct PairEquals {
		template <class T1, class T2>
		bool operator () (const std::pair<T1, T2>& pair1, const std::pair<T1, T2>& pair2) const
		{
			auto minmax_pair1 = std::minmax(pair1.first, pair1.second);
			auto minmax_pair2 = std::minmax(pair2.first, pair2.second);
			return (minmax_pair1.first == minmax_pair2.first) && (minmax_pair1.second == minmax_pair2.second);
		}
	};
	
	struct VectorHash {
		template <class T>
		std::size_t operator () (const std::vector<T>& vector) const
		{
			std::size_t seed = 0;
			std::vector<T> sorted_vector = vector;
			std::sort(sorted_vector.begin(), sorted_vector.end());

			for (auto value : sorted_vector)
			{
				boost::hash_combine(seed, value);
			}

			return seed;
		}

		std::size_t operator () (const Eigen::VectorXi& vector) const
		{
			const auto vector_internal = std::vector<int64_t>(vector.data(), vector.data() + vector.rows());
			return this->operator()(vector_internal);
		}
	};

	struct VectorEquals {
		template <class T>
		bool operator () (const std::vector<T>& vector1, const std::vector<T>& vector2) const
		{
			if (vector1.size() != vector2.size())
			{
				return false;
			}

			std::vector<T> sorted_vector1 = vector1;
			std::vector<T> sorted_vector2 = vector2;
			std::sort(sorted_vector1.begin(), sorted_vector1.end());
			std::sort(sorted_vector2.begin(), sorted_vector2.end());

			return sorted_vector1 == sorted_vector2;
		}

		bool operator () (const Eigen::VectorXi& vector1, const Eigen::VectorXi& vector2) const
		{
			const auto vector1_internal = std::vector<int64_t>(vector1.data(), vector1.data() + vector1.rows());
			const auto vector2_internal = std::vector<int64_t>(vector2.data(), vector2.data() + vector2.rows());
			return this->operator()(vector1_internal, vector2_internal);
		}
	};

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
	 * Sparse matrix creation
	 */
	template<int Scheme>
	static inline void SparseMatrixFromTriplets(
		const std::vector<int>& ii, 
		const std::vector<int>& jj, 
		const std::vector<double>& ss, 
		const Eigen::SparseMatrix<double>::Index rows, 
		const Eigen::SparseMatrix<double>::Index columns, 
		Eigen::SparseMatrix<double, Scheme>& M)
	{
		std::vector<Eigen::Triplet<double>> triplets;
		triplets.resize(ii.size());

		for (int i = 0; i < ii.size(); i++)
		{
			triplets[i] = Eigen::Triplet<double>(ii[i], jj[i], ss[i]);
		}

		M.resize(rows, columns);
		M.setFromTriplets(triplets.begin(), triplets.end());
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
};

#endif