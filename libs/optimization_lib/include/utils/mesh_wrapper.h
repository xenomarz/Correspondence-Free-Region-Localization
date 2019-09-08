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
	MeshWrapper(const Eigen::MatrixX3d& V, const Eigen::MatrixX3i& F);
	virtual ~MeshWrapper();

	const Eigen::MatrixX3i& GetF() const;
	const Eigen::MatrixX3d& GetV() const;
	const Eigen::MatrixX2i& GetE() const;
	const Eigen::MatrixX3i& GetFs() const;
	const Eigen::MatrixX2d& GetVs() const;
	const Eigen::MatrixX2i& GetEs() const;
	const Eigen::MatrixX3d& GetD1() const;
	const Eigen::MatrixX3d& GetD2() const;
	const Eigen::SparseMatrix<int>& GetV2V() const;
	const Eigen::SparseMatrix<int>& GetE2E() const;
	const Eigen::SparseMatrix<int>& GetV2E() const;
	const Eigen::SparseMatrix<int>& GetV2Es() const;

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
	using VIs2VIMap = std::unordered_map<VertexIndex, VertexIndex>;
	using EI2EIsMap = std::unordered_map<EdgeIndex, std::vector<EdgeIndex>>;
	using EIs2EIMap = std::unordered_map<EdgeIndex, EdgeIndex>;

	void ComputeSoup2DRandom(const Eigen::MatrixX3i& Fs, Eigen::MatrixX2d& Vs);
	void FixFlippedFaces(const Eigen::MatrixX3i& Fs, Eigen::MatrixX2d& Vs);
	void ComputeFs(const Eigen::MatrixX3i& F, Eigen::MatrixX3i& Fs);
	void ComputeE(const Eigen::MatrixX3i& F, Eigen::MatrixX2i& E);
	void ComputeED2EIMap(const Eigen::MatrixX2i& E, ED2EIMap& ed2ei);
	void ComputeVI2VIMaps(const Eigen::MatrixX3i& F, VI2VIsMap& vi2vis, VIs2VIMap& vis2vi);
	void ComputeEI2EIMaps(const Eigen::MatrixX2i& Es, const VIs2VIMap& vis2vi, const ED2EIMap& ed2ei, EI2EIsMap& ei2eis, EIs2EIMap& eis2ei);
	void ComputeCC(const Eigen::MatrixX2i& E, const Eigen::MatrixX2i& Es, const EI2EIsMap& ei2eis, const VIs2VIMap& vis2vi, Eigen::SparseMatrix<double>& CC);
	void ComputeSurfaceGradientPerFace(const Eigen::MatrixX3d& V, const Eigen::MatrixX3i& F, Eigen::MatrixX3d& D1, Eigen::MatrixX3d& D2);
	void NormalizeMesh(Eigen::MatrixX3d& V);

	unsigned int nv_;
	unsigned int nf_;
	unsigned int ne_;
	unsigned int nvs_;
	unsigned int nfs_;
	unsigned int nes_;

	Eigen::MatrixX3d V_;
	Eigen::MatrixX2d Vs_;
	Eigen::MatrixX3i F_;
	Eigen::MatrixX3i Fs_;
	Eigen::MatrixX2i E_;
	Eigen::MatrixX2i Es_;
	Eigen::MatrixX3d D1_;
	Eigen::MatrixX3d D2_;
	Eigen::SparseMatrix<int> V2V_;
	Eigen::SparseMatrix<int> E2E_;
	Eigen::SparseMatrix<int> V2E_;
	Eigen::SparseMatrix<int> V2Es_;

	// Corners correspondence matrix
	Eigen::SparseMatrix<double> CC_;

	ED2EIMap ed2ei_;
	VI2VIsMap vi2vis_;
	VIs2VIMap vis2vi_;
	EI2EIsMap ei2eis_;
	EIs2EIMap eis2ei_;
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif