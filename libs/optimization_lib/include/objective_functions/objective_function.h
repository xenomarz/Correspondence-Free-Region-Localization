#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H

// STL Includes
#include <memory>
#include <atomic>
#include <type_traits>
#include <string>
#include <mutex>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "../utils/objective_function_data_provider.h"
#include "../utils/utils.h"

template<Eigen::StorageOptions StorageOrder>
class ObjectiveFunction
{
public:
	/**
	 * Public type definitions
	 */
	enum class UpdateOptions : uint32_t
	{
		NONE = 0,
		VALUE = 1,
		GRADIENT = 2,
		HESSIAN = 4,
		ALL = 7
	};

	/**
	 * Constructor and destructor
	 */
	ObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
		f_(0),
		w_(1),
		domain_faces_count_(0),
		domain_vertices_count_(0),
		image_faces_count_(0),
		image_vertices_count_(0),
		variables_count_(0),
		name_(name),
		objective_function_data_provider_(objective_function_data_provider)
	{
		
	}

	virtual ~ObjectiveFunction()
	{
		// Empty implementation
	}

	/**
	 * Getters
	 */
	double GetValue() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return f_;
	}

	const Eigen::VectorXd& GetValuePerVertex() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return f_per_vertex_;
	}

	const Eigen::VectorXd& GetGradient() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return g_;
	}

	const Eigen::SparseMatrix<double, StorageOrder>& GetHessian() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return H_;
	}

	double GetWeight() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return w_;
	}

	std::string GetName() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return name_;
	}

	/**
	 * Setters
	 */
	void SetWeight(const double w)
	{
		std::lock_guard<std::mutex> lock(m_);
		w_ = w;
	}

	/**
	 * Public methods
	 */

	// Initializes the objective function object. Must be called from any derived class constructor.
	void Initialize()
	{
		domain_faces_count_ = objective_function_data_provider_->GetDomainFaces().rows();
		domain_vertices_count_ = objective_function_data_provider_->GetDomainVertices().rows();
		image_faces_count_ = objective_function_data_provider_->GetImageFaces().rows();
		image_vertices_count_ = objective_function_data_provider_->GetImageVerticesCount();
		variables_count_ = 2 * image_vertices_count_;
		
		PreInitialize();
		InitializeValue(f_, f_per_vertex_);
		InitializeGradient(g_);
		InitializeHessian(H_);
		PostInitialize();
	}

	// Update value, gradient and hessian for a given x
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options = UpdateOptions::ALL)
	{
		std::lock_guard<std::mutex> lock(m_);
		PreUpdate(x);

		if ((update_options & UpdateOptions::VALUE) != UpdateOptions::NONE)
		{
			CalculateValue(f_, f_per_vertex_);
		}

		if ((update_options & UpdateOptions::GRADIENT) != UpdateOptions::NONE)
		{
			CalculateGradient(g_);
		}

		if ((update_options & UpdateOptions::HESSIAN) != UpdateOptions::NONE)
		{
			CalculateHessian(H_);
		}

		PostUpdate(x);
	}

protected:

	/**
	 * Protected methods
	 */
	virtual void PreInitialize()
	{
		// Empty implementation
	}
	
	virtual void PostInitialize()
	{
		// Empty implementation
	}
	
	virtual void PreUpdate(const Eigen::VectorXd& x)
	{
		// Empty implementation
	}

	virtual void PostUpdate(const Eigen::VectorXd& x)
	{
		// Empty implementation
	}

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

	 // Value, gradient and hessian initializers
	void InitializeValue(double& f, Eigen::VectorXd& f_per_vertex)
	{
		f = 0;
		f_per_vertex.conservativeResize(image_vertices_count_);
		f_per_vertex.setZero();
	}

	void InitializeGradient(Eigen::VectorXd& g)
	{
		g.conservativeResize(variables_count_);
		g.setZero();
	}

	void InitializeHessian(Eigen::SparseMatrix<double, StorageOrder>& H)
	{
		H.resize(variables_count_, variables_count_);
	}

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) = 0;
	virtual void CalculateGradient(Eigen::VectorXd& g) = 0;
	virtual void CalculateHessian(Eigen::SparseMatrix<double, StorageOrder>& H) = 0;

	/**
	 * Private fields
	 */

	// Value
	double f_;
	Eigen::VectorXd f_per_vertex_;

	// Gradient
	Eigen::VectorXd g_;

	// Hessian
	Eigen::SparseMatrix<double, StorageOrder> H_;

	// Weight
	std::atomic<double> w_;

	// Name
	const std::string name_;
};

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor>::UpdateOptions rhs);

#endif