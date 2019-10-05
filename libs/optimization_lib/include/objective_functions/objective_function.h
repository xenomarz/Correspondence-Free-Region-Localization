#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H

// STL Includes
#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <type_traits>
#include <string>
#include <mutex>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "../utils/objective_function_data_provider.h"
#include "../utils/utils.h"

class ObjectiveFunction
{
public:

	/**
	 * Constructor and destructor
	 */
	ObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name);
	virtual ~ObjectiveFunction();

	/**
	 * Getters
	 */
	inline double GetValue() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return f_;
	}

	inline const Eigen::VectorXd& GetGradient() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return g_;
	}

	inline const std::vector<int>& GetII() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return ii_;
	}

	inline const std::vector<int>& GetJJ() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return jj_;
	}

	inline const std::vector<double>& GetSS() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return ss_;
	}

	template<Eigen::StorageOptions StorageOptions>
	inline const Eigen::SparseMatrix<double, StorageOptions>& GetHessian() const
	{
		std::lock_guard<std::mutex> lock(m_);
		switch (StorageOptions)
		{
		case Eigen::ColMajor:
			return H_;
		case Eigen::RowMajor:
			return H_rm_;
		}

		return H_;
	}

	inline double GetWeight() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return w_;
	}

	/**
	 * Setters
	 */
	void SetWeight(const double w);
	const std::string GetName() const;

	/**
	 * Public methods
	 */

	// Initializes the objective function object. Must be called from any derived class constructor.
	void Initialize();

	// Update value, gradient and hessian for a given x
	inline void Update(const Eigen::VectorXd& x)
	{
		std::lock_guard<std::mutex> lock(m_);
		if (IsValid())
		{
			PreUpdate(x);
			CalculateValue(x, f_);
			CalculateGradient(x, g_);
			CalculateHessian(x, ss_);
			//Utils::SparseMatrixFromTriplets(ii_, jj_, ss_, variables_count_, variables_count_, H_);
			Utils::SparseMatrixFromTriplets(ii_, jj_, ss_, variables_count_, variables_count_, H_rm_);
			PostUpdate(x);
		}
	}

	// Returns true is the objective function's state is valid, false otherwise
	virtual bool IsValid();

protected:

	/**
	 * Protected methods
	 */
	virtual void PreInitialize();
	virtual void PostInitialize();
	virtual void PreUpdate(const Eigen::VectorXd& x);
	virtual void PostUpdate(const Eigen::VectorXd& x);

	/**
	 * Protected Fields
	 */

	// Objective function data provider
	std::shared_ptr<ObjectiveFunctionDataProvider> objective_function_data_provider_;

	// Mutex
	mutable std::mutex m_;

	// Elements count
	Eigen::DenseIndex domain_faces_count_;
	Eigen::DenseIndex domain_vertices_count_;
	Eigen::DenseIndex image_faces_count_;
	Eigen::DenseIndex image_vertices_count_;
	Eigen::DenseIndex variables_count_;

private:

	/**
	 * Private methods
	 */

	// Gradient and hessian initializers
	virtual void InitializeGradient(Eigen::VectorXd& g);
	virtual void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) = 0;

	// Value, gradient and hessian calculation functions
	// TODO: Remove input argument 'const Eigen::VectorXd& x' from value, gradient and hessian calculation. This argument should be processed in PreUpdate().
	virtual void CalculateValue(const Eigen::VectorXd& x, double& f) = 0;
	virtual void CalculateGradient(const Eigen::VectorXd& x, Eigen::VectorXd& g) = 0;
	virtual void CalculateHessian(const Eigen::VectorXd& x, std::vector<double>& ss) = 0;

	/**
	 * Private fields
	 */

	// Value
	double f_;

	// Gradient
	Eigen::VectorXd g_;

	// Hessian (sparse representation)
	// TODO: remove ii_, jj_ and ss_ and use vector of triplets
	std::vector<int> ii_; 
	std::vector<int> jj_;
	std::vector<double> ss_;
	Eigen::SparseMatrix<double, Eigen::ColMajor> H_;
	Eigen::SparseMatrix<double, Eigen::RowMajor> H_rm_;

	// Weight
	std::atomic<double> w_;

	// Name
	const std::string name_;
};

#endif