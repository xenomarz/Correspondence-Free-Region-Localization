#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H

// STL Includes
#include <memory>
#include <atomic>
#include <type_traits>
#include <string>
#include <mutex>
#include <any>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "../utils/objective_function_data_provider.h"

template<Eigen::StorageOptions StorageOrder_, typename GradientType_>
class ObjectiveFunction
{
public:
	/**
	 * Public type definitions
	 */
	using GradientType = GradientType_;
	
	enum TemplateSettings
	{
		StorageOrder = StorageOrder_
	};
	
	enum class UpdateOptions : uint32_t
	{
		NONE = 0,
		VALUE = 1,
		GRADIENT = 2,
		HESSIAN = 4,
		ALL = 7
	};

	enum class Properties : uint32_t
	{
		Value,
		ValuePerVertex,
		Gradient,
		GradientNorm,
		Hessian,
		Weight,
		Name,
		Count_
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

	const GradientType_& GetGradient() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return g_;
	}

	const Eigen::SparseMatrix<double, StorageOrder_>& GetHessian() const
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

	// Generic property getter
	virtual bool GetProperty(const uint32_t property_id, std::any& property_value)
	{
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Value:
			property_value = GetName();
			return true;
		case Properties::GradientNorm:
			property_value = GetGradient().norm();
			return true;
		case Properties::Weight:
			property_value = GetWeight();
			return true;
		case Properties::ValuePerVertex:
			property_value = GetValuePerVertex();
			return true;
		case Properties::Gradient:
			property_value = GetGradient();
			return true;
		case Properties::Name:
			property_value = GetName();
			return true;
		case Properties::Hessian:
			property_value = GetHessian();
			return true;
		}

		return false;
	}

	/**
	 * Setters
	 */
	void SetWeight(const double w)
	{
		std::lock_guard<std::mutex> lock(m_);
		w_ = w;
	}

	// Generic property setter
	virtual bool SetProperty(const uint32_t property_id, const std::any& property_value)
	{
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Weight:
			SetWeight(std::any_cast<const double&>(property_value));
			return true;
		}

		return false;
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
	uint64_t domain_faces_count_;
	uint64_t domain_vertices_count_;
	uint64_t image_faces_count_;
	uint64_t image_vertices_count_;
	uint64_t variables_count_;

private:

	/**
	 * Private methods
	 */

	 // Value, gradient and hessian initializers
	void InitializeValue(double& f, Eigen::VectorXd& f_per_vertex)
	{
		f = 0;
		f_per_vertex.resize(image_vertices_count_);
		f_per_vertex.setZero();
	}

	void InitializeGradient(GradientType_& g)
	{
		g.resize(variables_count_);
		g.setZero();
	}

	void InitializeHessian(Eigen::SparseMatrix<double, StorageOrder_>& H)
	{
		H.resize(variables_count_, variables_count_);
	}

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) = 0;
	virtual void CalculateGradient(GradientType_& g) = 0;
	virtual void CalculateHessian(Eigen::SparseMatrix<double, StorageOrder_>& H) = 0;

	/**
	 * Private fields
	 */

	// Value
	double f_;
	Eigen::VectorXd f_per_vertex_;

	// Gradient
	GradientType_ g_;

	// Hessian
	Eigen::SparseMatrix<double, StorageOrder_> H_;

	// Weight
	std::atomic<double> w_;

	// Name
	const std::string name_;
};

// http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>::UpdateOptions rhs);

ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions operator | (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& operator |= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions operator & (const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions lhs, const ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs);
ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& operator &= (ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions& lhs, ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>::UpdateOptions rhs);

#endif