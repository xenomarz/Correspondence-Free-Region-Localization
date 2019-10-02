#pragma once
#ifndef OPTIMIZATION_LIB_MESH_WRAPPER_H
#define OPTIMIZATION_LIB_MESH_WRAPPER_H

// STL Includes
#include <vector>
#include <map>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>

// Boost includes
#include <boost/signals2/signal.hpp>


// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Optimization lib includes
#include "./objective_function_data_provider.h"

class MeshWrapper : public ObjectiveFunctionDataProvider
{
private:

	/**
	 * Private function objects
	 */

	 // Custom hash and equals functions for unordered_map
	 // https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& pair) const
		{
			auto minmax_pair = std::minmax(pair.first, pair.second);
			std::size_t seed = 0;
			hash_combine(seed, minmax_pair.first);
			hash_combine(seed, minmax_pair.second);
			return seed;
		}
	};

	struct pair_equals {
		template <class T1, class T2>
		bool operator () (const std::pair<T1, T2>& pair1, const std::pair<T1, T2>& pair2) const
		{
			auto minmax_pair1 = std::minmax(pair1.first, pair1.second);
			auto minmax_pair2 = std::minmax(pair2.first, pair2.second);
			return (minmax_pair1.first == minmax_pair2.first) && (minmax_pair1.second == minmax_pair2.second);
		}
	};

	/**
	 * Private type definitions
	 */
	using IndexType = Eigen::DenseIndex;
	using VertexIndex = IndexType;
	using EdgeIndex = IndexType;
	using FaceIndex = IndexType;
	using EdgeDescriptor = std::pair<EdgeIndex, EdgeIndex>;
	using ED2EIMap = std::unordered_map<EdgeDescriptor, EdgeIndex, pair_hash, pair_equals>;
	using VI2VIsMap = std::unordered_map<VertexIndex, std::vector<VertexIndex>>;
	using VI2VIMap = std::unordered_map<VertexIndex, VertexIndex>;
	using EI2EIsMap = std::unordered_map<EdgeIndex, std::vector<EdgeIndex>>;
	using EI2EIMap = std::unordered_map<EdgeIndex, EdgeIndex>;

	/**
	 * Private functions
	 */
	void Initialize();

public:

	/**
	 * Public type definitions
	 */
	enum class SoupInitType {
		RANDOM,
		ISOMETRIC
	};

	using ModelLoadedCallback = void();

	/**
	 * Constructors and destructor
	 */
	MeshWrapper();
	MeshWrapper(const Eigen::MatrixX3d& v, const Eigen::MatrixX3i& f);
	MeshWrapper(const std::string& modelFilename);
	virtual ~MeshWrapper();

	/**
	 * Getters
	 */
	const Eigen::MatrixX3i& GetImageFaces() const;
	const Eigen::MatrixX2d& GetImageVertices() const;
	const Eigen::MatrixX2i& GetImageEdges() const;

	/**
	 * Overrides
	 */
	const Eigen::MatrixX3i& GetDomainFaces() const;
	const Eigen::MatrixX3d& GetDomainVertices() const;
	const Eigen::MatrixX2i& GetDomainEdges() const;
	const Eigen::MatrixX3d& GetD1() const;
	const Eigen::MatrixX3d& GetD2() const;
	const Eigen::SparseMatrix<double>& GetCorrespondingVertexPairsCoefficients() const;
	const Eigen::VectorXd& GetCorrespondingVertexPairsEdgeLength() const;
	const Eigen::DenseIndex GetImageVerticesCount() const;

	/**
	 * Public methods
	 */
	Eigen::VectorXi GetImageFaceVerticesIndices(FaceIndex face_index);
	Eigen::MatrixXd GetImageVertices(const Eigen::VectorXi& vertex_indices);
	void LoadModel(const std::string& model_file_path);
	void RegisterModelLoadedCallback(std::function<ModelLoadedCallback> model_loaded_callback);

private:
	/**
	* Private enums
	*/
	enum class ModelFileType
	{
		OBJ,
		OFF,
		UNKNOWN
	};

	/**
	 * General use mesh methods
	 */
	void ComputeEdges(const Eigen::MatrixX3i& f, Eigen::MatrixX2i& e);
	void NormalizeVertices(Eigen::MatrixX3d& v);

	/**
	 * Discrete operators
	 */
	void ComputeSurfaceGradientPerFace(const Eigen::MatrixX3d& v, const Eigen::MatrixX3i& f, Eigen::MatrixX3d& d1, Eigen::MatrixX3d& d2);

	/**
	 * Triangle soup methods
	 */

	// Soup generation
	void GenerateSoupFaces(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out);
	void FixFlippedFaces(const Eigen::MatrixX3i& f_im, Eigen::MatrixX2d& v_im);
	void GenerateRandom2DSoup(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out, Eigen::MatrixX2d& v_out);

	// Edge descriptor -> edge index map
	void ComputeEdgeDescriptorMap(const Eigen::MatrixX2i& e, ED2EIMap& ed_2_ei);

	// Domain edge index <-> image edge index maps
	void ComputeEdgeIndexMaps();

	// Domain vertex index <-> image vertex index maps
	void ComputeVertexIndexMaps();

	// Image vertices corresponding pairs and image edges corresponding pairs
	void ComputeCorrespondingPairs();
	void ComputeCorrespondingVertexPairsCoefficients();
	void ComputeCorrespondingVertexPairsEdgeLength();

	/**
	 * Private methods
	 */
	ModelFileType GetModelFileType(const std::string& modelFilePath);

	/**
	 * Fields
	 */

	// Domain matrices
	Eigen::MatrixX3d v_dom_;
	Eigen::MatrixX3i f_dom_;
	Eigen::MatrixX2i e_dom_;

	// Image matrices
	Eigen::MatrixX2d v_im_;
	Eigen::MatrixX3i f_im_;
	Eigen::MatrixX2i e_im_;

	// Discrete partial-derivatives matrices
	Eigen::MatrixX3d d1_;
	Eigen::MatrixX3d d2_;

	// Image corresponding pairs
	std::vector<std::pair<VertexIndex, VertexIndex>> cv_pairs_;
	std::vector<std::pair<EdgeIndex, EdgeIndex>> ce_pairs_;
	Eigen::SparseMatrix<double> cv_pairs_coefficients_;
	Eigen::VectorXd cv_pairs_edge_length_;

	// Maps
	ED2EIMap ed_im_2_ei_im_;
	ED2EIMap ed_dom_2_ei_dom_;
	VI2VIsMap v_dom_2_v_im_;
	VI2VIMap v_im_2_v_dom_;
	EI2EIsMap e_dom_2_e_im_;
	EI2EIMap e_im_2_e_dom_;

	// Boost signals
	boost::signals2::signal<ModelLoadedCallback> model_loaded_signal_;
};

// https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values/35991300#35991300
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif