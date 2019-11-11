#pragma once
#ifndef OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H
#define OPTIMIZATION_LIB_OBJECTIVE_FUNCTION_H

// STL Includes
#include <memory>
#include <atomic>
#include <string>
#include <mutex>
#include <any>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "../utils/type_definitions.h"
#include "../utils/data_providers/mesh_data_provider.h"

template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
class ObjectiveFunction
{
public:
	/**
	 * Public type definitions
	 */
	using GradientType = VectorType_;
	
	enum TemplateSettings
	{
		StorageOrder = StorageOrder_
	};
	
	enum class UpdateOptions : int32_t
	{
		NONE = 0,
		VALUE = 1,
		GRADIENT = 2,
		HESSIAN = 4,
		ALL = 7
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
	ObjectiveFunction(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::string& name, const int64_t objective_vertices_count, const bool enforce_psd) :
		f_(0),
		w_(1),
		domain_faces_count_(0),
		domain_vertices_count_(0),
		image_faces_count_(0),
		image_vertices_count_(0),
		variables_count_(0),
		objective_vertices_count_(objective_vertices_count),
		objective_variables_count_(2 * objective_vertices_count),
		enforce_psd_(enforce_psd),
		name_(name),
		mesh_data_provider_(mesh_data_provider)
	{
		
	}

	virtual ~ObjectiveFunction()
	{
		// Empty implementation
	}

	/**
	 * Thread-safe getters
	 */
	double GetValue() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return f_;
	}

	const VectorType_& GetValuePerVertex() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return f_per_vertex_;
	}

	const VectorType_& GetGradient() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return g_;
	}

	const Eigen::SparseMatrix<double, StorageOrder_>& GetHessian()
	{
		std::lock_guard<std::mutex> lock(m_);

		// TODO: Add a flag that allows to explicitly zero the main diagonal
		for(int64_t i = 0; i < this->variables_count_; i++)
		{
			triplets_.push_back(Eigen::Triplet<double>(i,i,0));
		}
		
		H_.setFromTriplets(triplets_.begin(), triplets_.end());
		return H_;
	}

	const std::vector<Eigen::Triplet<double>>& GetTriplets() const
	{
		std::lock_guard<std::mutex> lock(m_);
		return triplets_;
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
	virtual bool GetProperty(const int32_t property_id, std::any& property_value)
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
		domain_faces_count_ = mesh_data_provider_->GetDomainFaces().rows();
		domain_vertices_count_ = mesh_data_provider_->GetDomainVertices().rows();
		image_faces_count_ = mesh_data_provider_->GetImageFaces().rows();
		image_vertices_count_ = mesh_data_provider_->GetImageVerticesCount();
		variables_count_ = 2 * image_vertices_count_;
		
		PreInitialize();
		InitializeValue(f_);
		InitializeValuePerVertex(f_per_vertex_);
		InitializeGradient(g_);
		InitializeHessian(H_);
		InitializeTriplets(triplets_);
		InitializeMappings(hessian_entry_to_triplet_index_map_, sparse_variable_index_to_dense_variable_index_map_);
		PostInitialize();
	}

	// Update value, gradient and hessian for a given x
	void Update(const Eigen::VectorXd& x, const UpdateOptions update_options = UpdateOptions::ALL)
	{
		std::lock_guard<std::mutex> lock(m_);
		PreUpdate(x);

		if ((update_options & UpdateOptions::VALUE) != UpdateOptions::NONE)
		{
			CalculateValue(f_);
			CalculateValuePerVertex(f_per_vertex_);
		}

		if ((update_options & UpdateOptions::GRADIENT) != UpdateOptions::NONE)
		{
			CalculateGradient(g_);
		}

		if ((update_options & UpdateOptions::HESSIAN) != UpdateOptions::NONE)
		{
			CalculateTriplets(triplets_);
			CalculateConvexTriplets(triplets_);
		}

		PostUpdate(x);
	}

	virtual void AddValuePerVertex(Eigen::VectorXd& f_per_vertex, const double w = 1) const
	{
		f_per_vertex += w * f_per_vertex_;
	}

	virtual void AddGradient(Eigen::VectorXd& g, const double w = 1) const
	{
		g += w * g_;
	}

	void AddValuePerVertexSafe(Eigen::VectorXd& f_per_vertex, const double w = 1) const
	{
		std::lock_guard<std::mutex> lock(m_);
		AddValuePerVertex(f_per_vertex, w);
	}

	void AddGradientSafe(Eigen::VectorXd& g, const double w = 1) const
	{
		std::lock_guard<std::mutex> lock(m_);
		AddGradient(g, w);
	}

	virtual void AddTriplets(std::vector<Eigen::Triplet<double>>& triplets, const double w = 1) const
	{
		std::lock_guard<std::mutex> lock(m_);
		const int64_t start_index = triplets.size();
		const int64_t end_index = start_index + triplets_.size();
		triplets.insert(triplets.end(), triplets_.begin(), triplets_.end());
		for(int64_t i = start_index; i < end_index; i++)
		{
			double& value = const_cast<double&>(triplets[i].value());
			value *= w;
		}
	}
	
protected:
	/**
	 * Protected type definitions
	 */
	using HessianEntryToTripletIndexMap = std::unordered_map<RDS::HessianEntry, RDS::HessianTripletIndex, RDS::HessianEntryHash, RDS::HessianEntryEquals>;
	using SparseVariableIndexToDenseVariableIndexMap = std::unordered_map<RDS::SparseVariableIndex, RDS::DenseVariableIndex>;
	
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
	 * Protected Getters
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
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;

	// Mutex
	mutable std::mutex m_;

	// Elements count
	int64_t domain_faces_count_;
	int64_t domain_vertices_count_;
	int64_t image_faces_count_;
	int64_t image_vertices_count_;
	int64_t variables_count_;
	int64_t objective_vertices_count_;
	int64_t objective_variables_count_;

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
		f_per_vertex.resize(image_vertices_count_);
		f_per_vertex.setZero();
	}

	void InitializeGradient(VectorType_& g)
	{
		g.resize(variables_count_);
		g.setZero();
	}

	void InitializeHessian(Eigen::SparseMatrix<double, StorageOrder_>& H)
	{
		H.resize(variables_count_, variables_count_);
	}

	virtual void InitializeMappings(HessianEntryToTripletIndexMap& hessian_entry_to_triplet_index_map, SparseVariableIndexToDenseVariableIndexMap& sparse_variable_index_to_dense_variable_index_map)
	{
		// Empty implementation
	}

	virtual void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;

	// Value, gradient and hessian calculation functions
	virtual void CalculateValue(double& f) = 0;
	virtual void CalculateValuePerVertex(VectorType_& f_per_vertex) { /* Empty implementation */ };
	virtual void CalculateGradient(VectorType_& g) = 0;
	virtual void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) = 0;
	void CalculateConvexTriplets(std::vector<Eigen::Triplet<double>>& triplets)
	{
		if (enforce_psd_)
		{
			Eigen::MatrixXd H;
			H.resize(objective_variables_count_, objective_variables_count_);
			H.setZero();
			
			for (std::size_t i = 0; i < triplets_count; i++)
			{
				auto row = sparse_variable_index_to_dense_variable_index_map_[triplets[i].row()];
				auto col = sparse_variable_index_to_dense_variable_index_map_[triplets[i].col()];
				auto value = triplets[i].value();
				H.coeffRef(row, col) = value;
				H.coeffRef(col, row) = value;
			}

			Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(H);
			Eigen::MatrixXd D = solver.eigenvalues().asDiagonal();
			Eigen::MatrixXd V = solver.eigenvectors();
			for (auto i = 0; i < objective_variables_count_; i++)
			{
				auto& value = D.coeffRef(i, i);
				if (value < 0)
				{
					value = 10e-8;
				}
			}

			H = V * D * V.inverse();

			for (auto column = 0; column < objective_variables_count_; column++)
			{
				for (auto row = 0; row <= column; row++)
				{
					const auto triplet_index = hessian_entry_to_triplet_index_map_[{row, column}];
					const_cast<double&>(triplets[triplet_index].value()) = H.coeffRef(row, column);
				}
			}
		}
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

	// Enforce PSD
	bool enforce_psd_;

	// Mappings
	HessianEntryToTripletIndexMap hessian_entry_to_triplet_index_map_;
	SparseVariableIndexToDenseVariableIndexMap sparse_variable_index_to_dense_variable_index_map_;

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