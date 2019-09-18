#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION

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

// We two phase dynamic-binding-during-initialization idiom for initializing gradient and hessian in base class
// More info: https://isocpp.org/wiki/faq/strange-inheritance#calling-virtuals-from-ctor-idiom
class ObjectiveFunction
{
public:
	// Factory function. Won't compile if Derived is not a subclass of ObjectiveFunction.
	template <class Derived>
	static std::enable_if_t<std::is_base_of<ObjectiveFunction, Derived>::value, std::unique_ptr<Derived>> Create(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name)
	{
		std::unique_ptr<Derived> objective_function(new Derived(objective_function_data_provider, name));
		objective_function->InitializeGradient();
		objective_function->InitializeHessian();
		return objective_function;
	}

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

	// Update value, gradient and hessian for a given x
	virtual void PreUpdate(const Eigen::MatrixX2d& x);
	virtual void Update(const Eigen::MatrixX2d& x);
	virtual void PostUpdate(const Eigen::MatrixX2d& x);

protected:

	/**
	 * Constructor and destructor
	 */

	ObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name);
	virtual ~ObjectiveFunction();

	/**
	 * Protected Fields
	 */

	// Objective function data provider
	std::shared_ptr<ObjectiveFunctionDataProvider> objective_function_data_provider_;

	// Mutex
	std::mutex m_;

	// Elements count
	const Eigen::DenseIndex domain_faces_count_;
	const Eigen::DenseIndex domain_vertices_count_;
	const Eigen::DenseIndex image_faces_count_;
	const Eigen::DenseIndex image_vertices_count_;
	const Eigen::DenseIndex variables_count_;

private:
	// Gradient and hessian initializers
	virtual void InitializeGradient(Eigen::VectorXd& g);
	virtual void InitializeHessian(std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss) = 0;

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(const Eigen::MatrixX2d& X, double& f) = 0;
	virtual void CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g) = 0;
	virtual void CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss) = 0;
	void CalculateHessianInternal(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, Eigen::SparseMatrix<double>& H);

	// Value
	double f_;

	// Gradient
	Eigen::VectorXd g_;

	// Hessian (sparse representation)
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