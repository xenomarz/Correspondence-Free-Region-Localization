#pragma once

#include <Eigen/Core>
#include <Eigen/Sparse>

#ifdef __cplusplus
#endif

class Utils
{
public:
	static inline void SSVD2x2(const Eigen::Matrix2d& A, Eigen::Matrix2d& U, Eigen::Matrix2d& S, Eigen::Matrix2d& V);
};