#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H

// STL Includes
#include <memory>
#include <atomic>
#include <string>
#include <mutex>
#include <any>
#include <limits>

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization Lib Includes
#include "../core/core.h"
#include "../core/updatable_object.h"
#include "./objective_function_base.h"
#include "../data_providers/data_provider.h"

template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
class ObjectiveFunction : public ObjectiveFunctionBase
{
public:
	/**
	 * Public type definitions
	 */
	enum TemplateSettings
	{
		StorageOrder = StorageOrder_
	};

	/**
	 * Constructor and destructor
	 */
	ObjectiveFunction(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name) :
		ObjectiveFunctionBase(mesh_data_provider),
		f_(0),
		w_(1),
		name_(name),
		data_provider_(data_provider)
	{
		if (std::dynamic_pointer_cast<EmptyDataProvider>(data_provider_) == nullptr)
		{
			this->dependencies_.push_back(data_provider_);
		}
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
		return f_;
	}

	const VectorType_& GetValuePerVertex() const
	{
		return f_per_vertex_;
	}

	const Eigen::VectorXd& GetImageValuePerEdge() const
	{
		return image_value_per_edge_;
	}

	const Eigen::VectorXd& GetDomainValuePerEdge() const
	{
		return domain_value_per_edge_;
	}

	const VectorType_& GetGradient() const
	{
		return g_;
	}

	const Eigen::SparseMatrix<double, StorageOrder_>& GetHessian()
	{
		H_.setFromTriplets(triplets_.begin(), triplets_.end());
		return H_;
	}

	const std::vector<Eigen::Triplet<double>>& GetTriplets() const
	{
		return triplets_;
	}

	double GetWeight() const
	{
		return w_;
	}

	std::string GetName() const
	{
		return name_;
	}

	std::shared_ptr<DataProvider> GetDataProvider() const
	{
		return data_provider_;
	}

	// Generic property getter
	virtual bool GetProperty(const int32_t property_id, const int32_t property_modifier_id, const std::any property_context, std::any& property_value) override
	{
		const ObjectiveFunctionBase::PropertyModifiers property_modifiers = static_cast<ObjectiveFunctionBase::PropertyModifiers>(property_modifier_id);
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Value:
			property_value = GetValue();
			return true;
		case Properties::ValuePerVertex:
			property_value = GetValuePerVertex();
			return true;
		case Properties::ValuePerEdge:
			property_value = GetValuePerEdge(property_modifiers);
			return true;		
		case Properties::Gradient:
			property_value = GetGradient();
			return true;
		case Properties::Hessian:
			property_value = GetHessian();
			return true;
		case Properties::GradientNorm:
			property_value = GetGradient().norm();
			return true;
		case Properties::Weight:
			property_value = GetWeight();
			return true;
		case Properties::Name:
			property_value = GetName();
			return true;
		}

		return false;
	}

	/**
	 * Setters
	 */
	void SetWeight(const double w)
	{
		w_ = w;
	}

	// Generic property setter
	virtual bool SetProperty(const int32_t property_id, const std::any property_context, const std::any property_value) override
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
		std::lock_guard<std::mutex> lock(mutex_);
		PreInitialize();
		InitializeValue(f_);
		InitializeValuePerVertex(f_per_vertex_);
		InitializeValuePerEdge(image_value_per_edge_, domain_value_per_edge_);
		InitializeGradient(g_);
		InitializeHessian(H_);
		InitializeTriplets(triplets_);
		PostInitialize();
		UpdatableObject::Initialize();
	}

	// Update value, gradient and hessian for a given x
	void Update(const Eigen::VectorXd& x) override
	{
		Update(x, static_cast<int32_t>(UpdateOptions::All));
	}
	
	void Update(const Eigen::VectorXd& x, const int32_t update_modifiers) override
	{
		PreUpdate(x);

		const UpdateOptions update_options = static_cast<UpdateOptions>(update_modifiers);

		if ((update_options & UpdateOptions::Value) != UpdateOptions::None)
		{
			CalculateValue(f_);
		}

		if ((update_options & UpdateOptions::ValuePerVertex) != UpdateOptions::None)
		{
			CalculateValuePerVertex(f_per_vertex_);
		}

		if ((update_options & UpdateOptions::ValuePerEdge) != UpdateOptions::None)
		{
			CalculateValuePerEdge(domain_value_per_edge_, image_value_per_edge_);
		}

		if ((update_options & UpdateOptions::Gradient) != UpdateOptions::None)
		{
			CalculateGradient(g_);
		}

		if ((update_options & UpdateOptions::Hessian) != UpdateOptions::None)
		{
			CalculateTriplets(triplets_);
		}

		PostUpdate(x);
	}

	void UpdateLayers(const Eigen::VectorXd& x)
	{
		UpdateLayers(x, UpdateOptions::All);
	}

	void UpdateLayers(const Eigen::VectorXd& x, const UpdateOptions update_options)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		int32_t update_modifiers = static_cast<int32_t>(update_options);
		const auto layers_count = dependency_layers_.size();
		for(std::size_t current_layer_index = 0; current_layer_index < layers_count; current_layer_index++)
		{
			const auto& current_layer = dependency_layers_[current_layer_index];
			const auto objects_count = current_layer.size();

			// HACK: Remove this 'if' branch once Separation and Symmetric Dirichlet
			// objectives are composed as sum of sub-objectives
			if(current_layer_index == 0)
			{
				#pragma omp parallel for
				for (long i = 2; i < objects_count; i++)
				{
					current_layer[i]->Update(x, update_modifiers);
				}

				if (objects_count > 0)
				{
					current_layer[0]->Update(x, update_modifiers);
				}

				if (objects_count > 1)
				{
					current_layer[1]->Update(x, update_modifiers);
				}
			}
			else
			{
				#pragma omp parallel for
				for (long i = 0; i < objects_count; i++)
				{
					current_layer[i]->Update(x, update_modifiers);
				}
			}
		}

		Update(x, update_modifiers);
	}

	template<typename ValueVectorType_>
	void AddValuePerVertex(ValueVectorType_& f_per_vertex, const double w = 1) const
	{
		f_per_vertex = f_per_vertex + w * f_per_vertex_;
	}

	template<typename GradientVectorType_>
	void AddGradient(GradientVectorType_& g, const double w = 1) const
	{
		g = g + w * g_;
	}

	template<typename ValueVectorType_>
	void AddValuePerVertexSafe(ValueVectorType_& f_per_vertex, const double w = 1) const
	{
		AddValuePerVertex(f_per_vertex, w);
	}

	template<typename GradientVectorType_>
	void AddGradientSafe(GradientVectorType_& g, const double w = 1) const
	{
		AddGradient(g, w);
	}

	virtual void AddTriplets(std::vector<Eigen::Triplet<double>>& triplets, const double w = 1) const
	{
		const int64_t start_index = triplets.size();
		const int64_t end_index = start_index + triplets_.size();
		triplets.insert(triplets.end(), triplets_.begin(), triplets_.end());
		for(int64_t i = start_index; i < end_index; i++)
		{
			double& value = const_cast<double&>(triplets[i].value());
			value *= w;
		}
	}

	/**
	 * Gradient and hessian approximation using finite differences
	 */
	template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
	static Eigen::VectorXd GetApproximatedGradient(const std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>>& objective_function, const Eigen::VectorXd& x)
	{
		Eigen::VectorXd g(x.rows());
		g.setZero();

		Eigen::VectorXd perturbation(x.rows());

		const double epsilon = CalculateEpsilon(x);
		const double epsilon2 = 2 * epsilon;

		for (int64_t i = 0; i < x.rows(); i++)
		{
			perturbation.setZero();
			perturbation.coeffRef(i) = epsilon;
			Eigen::VectorXd x_plus_eps = x + perturbation;
			Eigen::VectorXd x_minus_eps = x - perturbation;

			objective_function->UpdateLayers(x_plus_eps);
			const double value_plus = objective_function->GetValue();

			objective_function->UpdateLayers(x_minus_eps);
			const double value_minus = objective_function->GetValue();

			g.coeffRef(i) = (value_plus - value_minus) / epsilon2;
		}

		return g;
	}

	template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
	static Eigen::MatrixXd GetApproximatedHessian(const std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>>& objective_function, const Eigen::VectorXd& x)
	{
		Eigen::MatrixXd H(x.rows(), x.rows());
		H.setZero();

		Eigen::VectorXd perturbation(x.rows());

		const double epsilon = CalculateEpsilon(x);
		const double epsilon2 = 2 * epsilon;
		for (int64_t i = 0; i < x.rows(); i++)
		{
			perturbation.setZero();
			perturbation.coeffRef(i) = epsilon;
			Eigen::VectorXd x_plus_eps = x + perturbation;
			Eigen::VectorXd x_minus_eps = x - perturbation;

			objective_function->UpdateLayers(x_plus_eps);
			const Eigen::VectorXd g_plus = objective_function->GetGradient();

			objective_function->UpdateLayers(x_minus_eps);
			const Eigen::VectorXd g_minus = objective_function->GetGradient();

			H.row(i) = (g_plus - g_minus) / epsilon2;
		}

		return H;
	}
	
protected:
	/**
	 * Protected methods
	 */
	virtual void InitializeDependencyLayers()
	{
		
	}
	
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
	 * Protected fields
	 */
	
	// Mutex
	mutable std::mutex m_;

	// Data provider
	std::shared_ptr<DataProvider> data_provider_;
	
private:

	/**
	 * Private getters
	 */
	
	const Eigen::VectorXd& GetValuePerEdge(const ObjectiveFunctionBase::PropertyModifiers property_modifiers) const
	{
		switch (property_modifiers)
		{
		case ObjectiveFunctionBase::PropertyModifiers::Domain:
			return GetDomainValuePerEdge();
		case ObjectiveFunctionBase::PropertyModifiers::Image:
			return GetImageValuePerEdge();
		}
	}

	/**
	 * Private methods
	 */

	 // Value, gradient and hessian initializers
	void InitializeValue(double& f)
	{
		f = 0;
	}

	void InitializeValuePerVertex(VectorType_& f_per_vertex)
	{
		f_per_vertex.resize(mesh_data_provider_->GetImageVerticesCount());
		f_per_vertex.setZero();
	}

	void InitializeValuePerEdge(Eigen::VectorXd& image_value_per_edge, Eigen::VectorXd& domain_value_per_edge)
	{
		domain_value_per_edge.resize(mesh_data_provider_->GetDomainEdgesCount());
		domain_value_per_edge.setZero();
		
		image_value_per_edge.resize(mesh_data_provider_->GetImageEdgesCount());
		image_value_per_edge.setZero();
	}

	void InitializeGradient(VectorType_& g)
	{
		g.resize(mesh_data_provider_->GetVariablesCount());
		g.setZero();
	}

	void InitializeHessian(Eigen::SparseMatrix<double, StorageOrder_>& H)
	{
		auto variables_count = mesh_data_provider_->GetVariablesCount();
		H.resize(variables_count, variables_count);
	}

	virtual void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(double& f) = 0;
	virtual void CalculateValuePerVertex(VectorType_& f_per_vertex) = 0;

	// TODO: Should be moved to a new class that will be a super class for edge related objective functions
	virtual void CalculateValuePerEdge(Eigen::VectorXd& domain_value_per_edge, Eigen::VectorXd& image_value_per_edge) = 0;
	
	virtual void CalculateGradient(VectorType_& g) = 0;
	virtual void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;

	// Epsilon calculation for finite differences
	static double CalculateEpsilon(const Eigen::VectorXd& x)
	{
		const double machine_epsilon = std::numeric_limits<double>::epsilon();
		const double max = x.cwiseAbs().maxCoeff();
		return std::cbrt(machine_epsilon) * max;
	}

	/**
	 * Private fields
	 */

	// Mutex
	mutable std::mutex mutex_;

	// Value
	double f_;

	// Value per vertex
	VectorType_ f_per_vertex_;

	// Value per edge
	// TODO: Use generic VectorType_
	Eigen::VectorXd image_value_per_edge_;
	Eigen::VectorXd domain_value_per_edge_;

	// Gradient
	VectorType_ g_;

	// Triplets
	std::vector<Eigen::Triplet<double>> triplets_;

	// Hessian
	Eigen::SparseMatrix<double, StorageOrder_> H_;
	
	// Weight
	double w_;

	// Name
	const std::string name_;
};

#endif