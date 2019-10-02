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
	double GetValue() const;
	const Eigen::VectorXd& GetGradient() const;
	const std::vector<int>& GetII() const;
	const std::vector<int>& GetJJ() const;
	const std::vector<double>& GetSS() const;
	const Eigen::SparseMatrix<double>& GetHessian() const;
	double GetWeight() const;

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
	virtual void Update(const Eigen::VectorXd& x);

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
	Eigen::SparseMatrix<double> H_;

	// Weight
	std::atomic<double> w_;

	// Name
	const std::string name_;
};

#endif