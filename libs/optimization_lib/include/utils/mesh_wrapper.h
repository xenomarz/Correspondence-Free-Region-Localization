#pragma once
#ifndef MESH_WRAPPER_H
#define MESH_WRAPPER_H

// STL Includes
#include <vector>
#include <map>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <algorithm>
#include <functional>

// Eigen Includes
#include <Eigen/Core>
#include <Eigen/Sparse>

class MeshWrapper
{
public:
	enum class SoupInitType {
		RANDOM,
		ISOMETRIC
	};

	MeshWrapper();
	MeshWrapper(const Eigen::MatrixX3d& v_dom, const Eigen::MatrixX3i& f_dom);
	virtual ~MeshWrapper();

	const Eigen::MatrixX3i& GetDomainFaces() const;
	const Eigen::MatrixX3d& GetDomainVertices() const;
	const Eigen::MatrixX2i& GetDomainEdges() const;
	const Eigen::MatrixX3i& GetImageFaces() const;
	const Eigen::MatrixX2d& GetImageVertices() const;
	const Eigen::MatrixX2i& GetImageEdges() const;
	const Eigen::MatrixX3d& GetD1() const;
	const Eigen::MatrixX3d& GetD2() const;

private:

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

	using IndexType = Eigen::DenseIndex;
	using VertexIndex = IndexType;
	using EdgeIndex = IndexType;
	using EdgeDescriptor = std::pair<EdgeIndex, EdgeIndex>;
	using ED2EIMap = std::unordered_map<EdgeDescriptor, EdgeIndex, pair_hash, pair_equals>;
	using VI2VIsMap = std::unordered_map<VertexIndex, std::vector<VertexIndex>>;
	using VI2VIMap = std::unordered_map<VertexIndex, VertexIndex>;
	using EI2EIsMap = std::unordered_map<EdgeIndex, std::vector<EdgeIndex>>;
	using EI2EIMap = std::unordered_map<EdgeIndex, EdgeIndex>;

	void GenerateRandom2DSoup(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out, Eigen::MatrixX2d& v_out);
	void GenerateSoupFaces(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out);

	void FixFlippedFaces(const Eigen::MatrixX3i& f_im, Eigen::MatrixX2d& v_im);
	void ComputeDomainEdges(const Eigen::MatrixX3i& f_dom, Eigen::MatrixX2i& e_dom);
	void ComputeED2EIMap(const Eigen::MatrixX2i& e_dom, ED2EIMap& ed_dom2ei_dom);
	void ComputeVI2VIMaps(const Eigen::MatrixX3i& f_dom, VI2VIsMap& v_dom2v_im, VI2VIMap& v_im2v_dom);
	void ComputeEI2EIMaps(const Eigen::MatrixX2i& e_im, const VI2VIMap& v_im2v_dom, const ED2EIMap& ed_dom2ei_dom, EI2EIsMap& e_dom2e_im, EI2EIMap& e_im2e_dom);
	void ComputeCornerCorrespondences(const Eigen::MatrixX2i& e_dom, const Eigen::MatrixX2i& e_im, const EI2EIsMap& e_dom2e_im, const VI2VIMap& v_im2v_dom, Eigen::SparseMatrix<double>& cc);
	void ComputeSurfaceGradientPerFace(const Eigen::MatrixX3d& v_dom, const Eigen::MatrixX3i& f_dom, Eigen::MatrixX3d& d1, Eigen::MatrixX3d& d2);
	void NormalizeVertices(Eigen::MatrixX3d& v);

	Eigen::MatrixX3d v_dom_;
	Eigen::MatrixX2d v_im_;
	Eigen::MatrixX3i f_dom_;
	Eigen::MatrixX3i f_im_;
	Eigen::MatrixX2i e_dom_;
	Eigen::MatrixX2i e_im_;
	Eigen::MatrixX3d d1_;
	Eigen::MatrixX3d d2_;

	// Corners correspondence matrix
	Eigen::SparseMatrix<double> cc_;

	ED2EIMap	ed_dom2ei_dom_;
	VI2VIsMap	v_dom2v_im_;
	VI2VIMap	v_im2v_dom_;
	EI2EIsMap	e_dom2e_im_;
	EI2EIMap	e_im2e_dom_;
};

// https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values/35991300#35991300
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif