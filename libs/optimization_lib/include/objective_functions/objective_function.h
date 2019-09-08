#pragma once

// STL Includes
#include <vector>
#include <memory>
#include <functional>
#include <type_traits>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "utils/mesh_wrapper.h"

// We two phase dynamic-binding-during-initialization idiom for initializing gradient and hessian in base class
// More info: https://isocpp.org/wiki/faq/strange-inheritance#calling-virtuals-from-ctor-idiom
class ObjectiveFunction
{
public:
	// Factory function. Won't compile if Derived is not a subclass of ObjectiveFunction.
	template <class Derived>
	static std::enable_if_t<std::is_base_of<ObjectiveFunction, Derived>::value, std::unique_ptr<Derived>> Create(const std::shared_ptr<MeshWrapper>& mesh_wrapper)
	{
		std::unique_ptr<Derived> objective_function(new Derived(mesh_wrapper));
		objective_function->InitializeGradient();
		objective_function->InitializeHessian();
		return objective_function;
	}

	// Value, gradient and hessian getters
	double GetValue() const;
	const Eigen::VectorXd& GetGradient() const;
	const std::vector<int>& GetII() const;
	const std::vector<int>& GetJJ() const;
	const std::vector<double>& GetSS() const;
	const MeshWrapper& GetMeshWrapper() const;

	// Update value, gradient and hessian for a given x
	void Update(const Eigen::MatrixX2d& X);

protected:
	// Protected constructor and destructor
	ObjectiveFunction(const std::shared_ptr<MeshWrapper>& mesh_wrapper);
	virtual ~ObjectiveFunction();

private:
	// Gradient and hessian initializers
	virtual void InitializeGradient(const std::shared_ptr<MeshWrapper>& mesh_wrapper, Eigen::VectorXd& g);
	virtual void InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& II, std::vector<int>& JJ, std::vector<double>& SS) = 0;

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(const Eigen::MatrixX2d& X, double& f) = 0;
	virtual void CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g) = 0;
	virtual void CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& SS) = 0;

	// Mesh Wrapper
	std::shared_ptr<MeshWrapper> mesh_wrapper_;

	// Value
	double f_;

	// Gradient
	Eigen::VectorXd g_;

	// Hessian (sparse representation)
	std::vector<int> II_; 
	std::vector<int> JJ_;
	std::vector<double> SS_;
};

