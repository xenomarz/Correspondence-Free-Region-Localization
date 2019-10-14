// Optimization lib includes
#include <objective_functions/objective_function.h>

ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions>(static_cast<T>(lhs)& static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs)
{
	lhs = lhs & rhs;
	return lhs;
}