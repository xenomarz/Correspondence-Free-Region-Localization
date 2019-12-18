// Optimization lib includes
#include <objective_functions/objective_function.h>

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions>(static_cast<T>(lhs)& static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs)
{
	using T = std::underlying_type_t <ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions>;
	return static_cast<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions>(static_cast<T>(lhs)& static_cast<T>(rhs));
}

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs)
{
	lhs = lhs & rhs;
	return lhs;
}