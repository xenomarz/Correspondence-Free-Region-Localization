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
#include "../data_providers/data_provider.h"

template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
class ObjectiveFunction : public UpdatableObject
{
public:
	/**
	 * Public type definitions
	 */
	enum TemplateSettings
	{
		StorageOrder = StorageOrder_
	};

	enum class Properties : int32_t
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
	ObjectiveFunction(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<DataProvider>& data_provider, const std::string& name) :
		UpdatableObject(mesh_data_provider),
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
	virtual bool GetProperty(const int32_t property_id, std::any& property_value)
	{
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Value:
			property_value = GetValue();
			return true;
		case Properties::ValuePerVertex:
			property_value = GetValuePerVertex();
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
	virtual bool SetProperty(const int32_t property_id, const std::any& property_value)
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
		PreInitialize();
		InitializeValue(f_);
		InitializeValuePerVertex(f_per_vertex_);
		InitializeGradient(g_);
		InitializeHessian(H_);
		InitializeTriplets(triplets_);
		PostInitialize();
		UpdatableObject::Initialize();
	}

	// Update value, gradient and hessian for a given x
	void Update(const Eigen::VectorXd& x) override
	{
		Update(x, UpdateOptions::ALL);
	}
	
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options)
	{
		PreUpdate(x);

		if ((update_options & UpdateOptions::VALUE) != UpdateOptions::NONE)
		{
			CalculateValue(f_);
		}

		if ((update_options & UpdateOptions::VALUE_PER_VERTEX) != UpdateOptions::NONE)
		{
			CalculateValuePerVertex(f_per_vertex_);
		}

		if ((update_options & UpdateOptions::GRADIENT) != UpdateOptions::NONE)
		{
			CalculateGradient(g_);
		}

		if ((update_options & UpdateOptions::HESSIAN) != UpdateOptions::NONE)
		{
			CalculateTriplets(triplets_);
		}

		PostUpdate(x);
	}

	void UpdateLayers(const Eigen::VectorXd& x)
	{
		UpdateLayers(x, UpdateOptions::ALL);
	}

	void UpdateLayers(const Eigen::VectorXd& x, const UpdateOptions update_options)
	{
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
					current_layer[i]->Update(x, update_options);
				}

				if (objects_count > 0)
				{
					current_layer[0]->Update(x, update_options);
				}

				if (objects_count > 1)
				{
					current_layer[1]->Update(x, update_options);
				}
			}
			else
			{
				#pragma omp parallel for
				for (long i = 0; i < objects_count; i++)
				{
					current_layer[i]->Update(x, update_options);
				}
			}
		}

		Update(x, update_options);
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
	 * Protected getters
	 */
	double GetValueInternal() const
	{
		return f_;
	}

	const VectorType_& GetValuePerVertexInternal() const
	{
		return f_per_vertex_;
	}

	const VectorType_& GetGradientInternal() const
	{
		return g_;
	}

	const Eigen::SparseMatrix<double, StorageOrder_>& GetHessianInternal() const
	{
		return H_;
	}

	double GetWeightInternal() const
	{
		return w_;
	}

	std::string GetNameInternal() const
	{
		return name_;
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

	// Value
	double f_;

	// Value per vertex
	VectorType_ f_per_vertex_;

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