#pragma once
#ifndef ENGINE_H
#define ENGINE_H

// Node API Includes
#include <napi.h>

// STL Includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include <libs/optimization_lib/include/utils/mesh_wrapper.h>
#include <libs/optimization_lib/include/solvers/eigen_sparse_solver.h>
#include <libs/optimization_lib/include/solvers/pardiso_solver.h>
#include <libs/optimization_lib/include/iterative_methods/newton_method.h>
#include <libs/optimization_lib/include/objective_functions/composite_objective.h>
#include <libs/optimization_lib/include/objective_functions/position.h>
#include <libs/optimization_lib/include/objective_functions/separation.h>
#include <libs/optimization_lib/include/objective_functions/symmetric_dirichlet.h>

class Engine : public Napi::ObjectWrap<Engine> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Engine(const Napi::CallbackInfo& info);

private:
	/**
	 * Private type definitions
	 */
	enum class ModelFileType
	{
		OBJ,
		OFF,
		UNKNOWN
	};

	enum class BufferedPrimitiveType : uint32_t
	{
		VERTEX = 0,
		TRIANGLE
	};

	enum class VerticesSource
	{
		DOMAIN_VERTICES,
		IMAGE_VERTICES
	};

	static Napi::FunctionReference constructor;

	/**
	 * NAPI private instance setters
	 */
	void SetPositionWeight(const Napi::CallbackInfo& info, const Napi::Value& value);
	void SetSeamlessWeight(const Napi::CallbackInfo& info, const Napi::Value& value);
	void SetLambda(const Napi::CallbackInfo& info, const Napi::Value& value);
	void SetDelta(const Napi::CallbackInfo& info, const Napi::Value& value);

	/**
	 * NAPI private instance getters
	 */
	Napi::Value GetPositionWeight(const Napi::CallbackInfo& info);
	Napi::Value GetSeamlessWeight(const Napi::CallbackInfo& info);
	Napi::Value GetLambda(const Napi::CallbackInfo& info);
	Napi::Value GetDelta(const Napi::CallbackInfo& info);
	Napi::Value GetObjectiveFunctionsData(const Napi::CallbackInfo& info);

	/**
	 * NAPI private instance methods
	 */
	Napi::Value GetDomainFaces(const Napi::CallbackInfo& info);
	Napi::Value GetImageFaces(const Napi::CallbackInfo& info);
	Napi::Value GetDomainVertices(const Napi::CallbackInfo& info);
	Napi::Value GetImageVertices(const Napi::CallbackInfo& info);
	Napi::Value GetDomainBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetImageBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetDomainBufferedUvs(const Napi::CallbackInfo& info);
	Napi::Value GetImageBufferedUvs(const Napi::CallbackInfo& info);
	Napi::Value LoadModel(const Napi::CallbackInfo& info);
	Napi::Value ResumeSolver(const Napi::CallbackInfo& info);
	Napi::Value PauseSolver(const Napi::CallbackInfo& info);
	Napi::Value ConstrainFacePosition(const Napi::CallbackInfo& info);
	Napi::Value UpdateConstrainedFacePosition(const Napi::CallbackInfo& info);
	Napi::Value UnconstrainFacePosition(const Napi::CallbackInfo& info);

	/**
	 * Regular private instance methods
	 */
	ModelFileType GetModelFileType(std::string filename);
	Napi::Array CreateFaces(Napi::Env env, const Eigen::MatrixX3i& F);
	Napi::Float32Array GetBufferedVertices(const Napi::CallbackInfo& info, const VerticesSource vertices_source);
	void TryUpdateImageVertices();

	/**
	 * Regular private templated instance methods
	 */
	template <typename Derived>
	Napi::Array CreateVerticesArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V)
	{
		Napi::Array vertices_array = Napi::Array::New(env, V.rows());
		auto entries_per_vertex = V.cols();
		for (int vertex_index = 0; vertex_index < V.rows(); vertex_index++)
		{
			Napi::Object vertex_object = Napi::Object::New(env);
			float x = V(vertex_index, 0);
			float y = V(vertex_index, 1);

			vertex_object.Set("x", x);
			vertex_object.Set("y", y);

			if (entries_per_vertex == 3)
			{
				float z = V(vertex_index, 2);
				vertex_object.Set("z", z);
			}
		
			vertices_array[vertex_index] = vertex_object;
		}

		return vertices_array;
	}

	template <typename Derived>
	Napi::Float32Array CreateBufferedVerticesArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V)
	{
		Napi::Float32Array buffered_vertices_array = Napi::Float32Array::New(env, 3 * V.rows());
		uint32_t entries_per_vertex = V.cols();

		#pragma omp parallel for
		for (uint32_t vertex_index = 0; vertex_index < V.rows(); vertex_index++)
		{
			float x = V(vertex_index, 0);
			float y = V(vertex_index, 1);
			float z = entries_per_vertex == 2 ? 0 : V(vertex_index, 2);

			int base_index = 3 * vertex_index;
			buffered_vertices_array[base_index] = x;
			buffered_vertices_array[base_index + 1] = y;
			buffered_vertices_array[base_index + 2] = z;
		}

		return buffered_vertices_array;
	}

	template <typename Derived>
	Napi::Float32Array CreateBufferedVerticesArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V, const Eigen::MatrixXi& F)
	{
		uint32_t entries_per_face = 9;
		uint32_t entries_per_vertex = V.cols();
		Napi::Float32Array buffered_vertices_array = Napi::Float32Array::New(env, entries_per_face * F.rows());

		#pragma omp parallel for
		for (uint32_t face_index = 0; face_index < F.rows(); face_index++)
		{
			int base_index = entries_per_face * face_index;
			for (uint32_t i = 0; i < 3; i++)
			{
				uint32_t vertex_index = F(face_index, i);
				float x = V(vertex_index, 0);
				float y = V(vertex_index, 1);
				float z = entries_per_vertex == 2 ? 0 : V(vertex_index, 2);
				
				int base_vertex_index = base_index + 3 * i;
				buffered_vertices_array[base_vertex_index] = x;
				buffered_vertices_array[base_vertex_index + 1] = y;
				buffered_vertices_array[base_vertex_index + 2] = z;
			}
		}

		return buffered_vertices_array;
	}

	template <typename Derived>
	Napi::Float32Array CreateBufferedUvsArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V, const Eigen::MatrixXi& F)
	{
		uint32_t entries_per_face = 6;
		uint32_t entries_per_vertex = V.cols();
		Napi::Float32Array buffered_uvs_array = Napi::Float32Array::New(env, entries_per_face * F.rows());

		#pragma omp parallel for
		for (uint32_t face_index = 0; face_index < F.rows(); face_index++)
		{
			int base_index = entries_per_face * face_index;
			for (uint32_t i = 0; i < 3; i++)
			{
				uint32_t vertex_index = F(face_index, i);
				float x = V(vertex_index, 0);
				float y = V(vertex_index, 1);

				int base_vertex_index = base_index + 2 * i;
				buffered_uvs_array[base_vertex_index] = x;
				buffered_uvs_array[base_vertex_index + 1] = y;
			}
		}

		return buffered_uvs_array;
	}

	/**
	 * Fields
	 */
	std::shared_ptr<MeshWrapper> mesh_wrapper_;
	std::shared_ptr<CompositeObjective> composite_objective_;
	std::shared_ptr<Position> position_;
	std::shared_ptr<Separation> separation_;
	std::shared_ptr<SymmetricDirichlet> symmetric_dirichlet_;
	std::unique_ptr<NewtonMethod<PardisoSolver, Eigen::StorageOptions::RowMajor>> newton_method_;
	std::vector<Eigen::DenseIndex> constrained_faces_indices;
	Eigen::MatrixX2d image_vertices_;
};

#endif