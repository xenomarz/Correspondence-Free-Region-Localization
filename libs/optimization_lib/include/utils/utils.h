#pragma once
#ifndef OPTIMIZATION_LIB_UTILS_H
#define OPTIMIZATION_LIB_UTILS_H

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class Utils
{
public:
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

	template<int Scheme>
	static inline void SparseMatrixFromTriplets(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, Eigen::SparseMatrix<double, Scheme>& M)
	{
		std::vector<Eigen::Triplet<double>> triplets;
		triplets.reserve(ii.size());
		int rows = *std::max_element(ii.begin(), ii.end()) + 1;
		int cols = *std::max_element(jj.begin(), jj.end()) + 1;

		for (int i = 0; i < ii.size(); i++)
		{
			triplets.push_back(Eigen::Triplet<double>(ii[i], jj[i], ss[i]));
		}

		M.resize(rows, cols);
		M.setFromTriplets(triplets.begin(), triplets.end());
	}

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
};

#endif