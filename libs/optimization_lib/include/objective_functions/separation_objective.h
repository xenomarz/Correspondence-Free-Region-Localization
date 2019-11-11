#pragma once
#ifndef OPTIMIZATION_LIB_SEPARATION_OBJECTIVE_H
#define OPTIMIZATION_LIB_SEPARATION_OBJECTIVE_H

// STL includes
#include <vector>

// Optimization lib includes
#include "./dense_objective_function.h"

template<Eigen::StorageOptions StorageOrder_>
class Separation : public DenseObjectiveFunction<StorageOrder_>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Delta = DenseObjectiveFunction<StorageOrder_>::Properties::Count_
	};
	
	/**
	 * Constructors and destructor
	 */
	Separation(const std::shared_ptr<MeshDataProvider>& mesh_data_provider) :
		DenseObjectiveFunction(mesh_data_provider, "Separation", 0, false)
	{
		this->Initialize();
	}

	virtual ~Separation()
	{

	}

	/**
	 * Setters
	 */
	void SetDelta(const double delta)
	{
		delta_ = delta;
	}

	bool SetProperty(const int32_t property_id, const std::any& property_value) override
	{
		if(DenseObjectiveFunction<StorageOrder_>::SetProperty(property_id, property_value))
		{
			return true;
		}
		
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Delta:
			SetDelta(std::any_cast<const double>(property_value));
			return true;
		}

		return false;
	}

	/**
	 * Getters
	 */
	double GetDelta() const
	{
		return delta_;
	}

	bool GetProperty(const int32_t property_id, std::any& property_value) override
	{
		if (DenseObjectiveFunction<StorageOrder_>::GetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Delta:
			property_value = GetDelta();
			return true;
		}

		return false;
	}

private:

	/**
	 * Overrides
	 */
	void CalculateValue(double& f) override
	{
		EsepP = Esep * X;
		EsepP_squared_rowwise_sum = EsepP.array().pow(2.0).rowwise().sum();
		EsepP_squared_rowwise_sum_plus_delta = EsepP_squared_rowwise_sum.array() + delta_;
		f_per_pair = EsepP_squared_rowwise_sum.cwiseQuotient(EsepP_squared_rowwise_sum_plus_delta);

		// add edge length factor
		f_per_pair = f_per_pair.cwiseProduct(edge_lenghts_per_pair);

		// sum everything up
		f = f_per_pair.sum();
	}

	void CalculateValuePerVertex(Eigen::VectorXd& f_per_vertex) override
	{
		f_per_vertex.setZero();
		int64_t vertex1_index;
		int64_t vertex2_index;
		
		#pragma omp parallel for
		for (int i = 0; i < Esept.outerSize(); ++i)
		{
			// no inner loop because there are only 2 nnz values per col
			Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
			int64_t vertex1_index = it.row();
			int64_t vertex2_index = (++it).row();

			f_per_vertex.coeffRef(vertex1_index) += EsepP_squared_rowwise_sum[i];
			f_per_vertex.coeffRef(vertex2_index) += EsepP_squared_rowwise_sum[i];
		}
	}
	
	void CalculateGradient(Eigen::VectorXd& g) override
	{
		Eigen::MatrixX2d ge;
		Eigen::VectorXd d_vec = Eigen::VectorXd::Constant(EsepP_squared_rowwise_sum.rows(), delta_);
		Eigen::VectorXd x_plus_d = EsepP_squared_rowwise_sum + d_vec;
		Eigen::VectorXd d = d_vec.cwiseQuotient(x_plus_d.cwiseAbs2());
		ge = 2.0 * Esept * d.cwiseProduct(edge_lenghts_per_pair).asDiagonal() * EsepP;
		g = Eigen::Map<Eigen::VectorXd>(ge.data(), 2.0 * ge.rows(), 1);
	}
	
	void PreUpdate(const Eigen::VectorXd& x) override
	{
		X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
	}
	
	void PreInitialize() override
	{
		Esep = this->mesh_data_provider_->GetCorrespondingVertexPairsCoefficients();
		Esept = Esep.transpose();
		edge_lenghts_per_pair = this->mesh_data_provider_->GetCorrespondingVertexPairsEdgeLength();
	}
	
	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		const int64_t outer_size = Esept.outerSize();
		triplets.reserve(10 * outer_size);
		for (int i = 0; i < outer_size; ++i)
		{
			Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
			int idx_xi = it.row();
			int idx_xj = (++it).row();

			// The indices in the small hessians are setup like this:
			// xi, xi+n, xj, xj+n from top to bottom and left to right
			// we traverse only the upper diagonal of each 4x4 hessian
			// and thus store 10 values, gathered in column order.

			// First column
			triplets.push_back(Eigen::Triplet<double>(idx_xi, idx_xi, 0));

			// Second column
			triplets.push_back(Eigen::Triplet<double>(idx_xi, idx_xi + this->image_vertices_count_, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xi + this->image_vertices_count_, idx_xi + this->image_vertices_count_, 0));

			// Third column
			triplets.push_back(Eigen::Triplet<double>(idx_xi, idx_xj, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xj, idx_xi + this->image_vertices_count_, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xj, idx_xj, 0));

			// Fourth column
			triplets.push_back(Eigen::Triplet<double>(idx_xi, idx_xj + this->image_vertices_count_, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xi + this->image_vertices_count_, idx_xj + this->image_vertices_count_, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xj, idx_xj + this->image_vertices_count_, 0));
			triplets.push_back(Eigen::Triplet<double>(idx_xj + this->image_vertices_count_, idx_xj + this->image_vertices_count_, 0));
		}
	}
	
	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		// no inner loop because there are only 2 nnz values per col
		#pragma omp parallel for
		for (int i = 0; i < Esept.outerSize(); ++i)
		{
			int tid = omp_get_thread_num();

			Eigen::Vector2d xi, xj;
			Eigen::Matrix4d sh;

			int idx_xi;
			int idx_xj;
			int factor;

			Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
			idx_xi = it.row();
			factor = it.value();
			idx_xj = (++it).row();
			xi = X.row(idx_xi);
			xj = X.row(idx_xj);
			FindSingleHessian(xi, xj, sh);
			sh *= factor;

			Eigen::Matrix4d Esep4;
			Esep4 << 1,  0, -1,  0,
					 0,  1,  0, -1,
					-1,  0,  1,  0,
					 0, -1,  0,  1;

			sh += Esep4;
			sh *= edge_lenghts_per_pair(i);

			int ind = 10 * i;
			for (int a = 0; a < 4; ++a)
			{
				for (int b = 0; b <= a; ++b)
				{
					const_cast<double&>(triplets[ind++].value()) = sh(b, a);
				}
			}
		}
	}
	
	/**
	 * Private methods
	 */
	void FindSingleHessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& H)
	{
		bool speedup = true;
		Eigen::Vector2d dx = xi - xj;
		Eigen::Vector4d dxx;
		dxx << dx, -dx;
		double t = 0.5 * dx.squaredNorm();
		double fp, fpp;

		fp = delta_ / ((t + delta_) * (t + delta_));
		Eigen::Matrix4d Esep4;
		Esep4 << 1,  0, -1,  0,
				 0,  1,  0, -1,
				-1,  0,  1,  0,
				 0, -1,  0,  1;

		H = fp * Esep4;
	}

	/**
	 * Fields
	 */
	double delta_ = 1.0;

	Eigen::MatrixX2d X;

	Eigen::SparseMatrix<double> Esep;
	Eigen::SparseMatrix<double> Esept;
	Eigen::MatrixX2d EsepP;

	Eigen::VectorXd f_per_pair;
	Eigen::VectorXd edge_lenghts_per_pair;
	Eigen::VectorXd EsepP_squared_rowwise_sum;
	Eigen::VectorXd EsepP_squared_rowwise_sum_plus_delta;
};

#endif