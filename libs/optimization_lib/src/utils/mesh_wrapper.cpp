// Optimization Library Includes
#include "utils/mesh_wrapper.h"

// LIBIGL Includes
#include <igl/slice.h>
#include <igl/local_basis.h>
#include <igl/doublearea.h>
#include <igl/per_face_normals.h>
#include <igl/adjacency_matrix.h>

MeshWrapper::MeshWrapper()
{

}

MeshWrapper::MeshWrapper(const Eigen::MatrixX3d& V, const Eigen::MatrixX3i& F) :
	V_(V),
	F_(F),
	nv_(V.rows()),
	nf_(F.rows()),
	nfs_(3 * nf_)
{
	NormalizeMesh(V_);
	ComputeFs(F_, Fs_);
	ComputeSoup2DRandom(Fs_, Vs_);
	ComputeE(F_, E_);
	ComputeE(Fs_, Es_);
	ComputeED2EIMap(E_, ed2ei_);
	ComputeVI2VIMaps(F_, vi2vis_, vis2vi_);
	ComputeEI2EIMaps(Es_, vis2vi_, ed2ei_, ei2eis_, eis2ei_);
	ComputeCC(E_, Es_, ei2eis_, vis2vi_, CC_);
	ComputeSurfaceGradientPerFace(V_, F_, D1_, D2_);
}

MeshWrapper::~MeshWrapper()
{

}

const Eigen::MatrixX3d& MeshWrapper::GetV() const
{
	return V_;
}

const Eigen::MatrixX3i& MeshWrapper::GetF() const
{
	return F_;
}

const Eigen::MatrixX2d& MeshWrapper::GetVs() const
{
	return Vs_;
}

const Eigen::MatrixX3i& MeshWrapper::GetFs() const
{
	return Fs_;
}

const Eigen::MatrixX2i& MeshWrapper::GetE() const
{
	return E_;
}

const Eigen::MatrixX2i& MeshWrapper::GetEs() const
{
	return Es_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD1() const
{
	return D1_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD2() const
{
	return D2_;
}

const Eigen::SparseMatrix<int>& MeshWrapper::GetV2V() const
{
	return V2V_;
}

const Eigen::SparseMatrix<int>& MeshWrapper::GetE2E() const
{
	return E2E_;
}

const Eigen::SparseMatrix<int>& MeshWrapper::GetV2E() const
{
	return V2E_;
}

const Eigen::SparseMatrix<int>& MeshWrapper::GetV2Es() const
{
	return V2Es_;
}

void MeshWrapper::FixFlippedFaces(const Eigen::MatrixX3i& Fs, Eigen::MatrixX2d& Vs)
{
	Eigen::Matrix<double, 3, 2> face_vertices;
	for (Eigen::MatrixX3i::Index i = 0; i < Fs.rows(); ++i)
	{
		igl::slice(Vs, Fs.row(i), 1, face_vertices);
		Eigen::Vector2d v1_2d = face_vertices.row(1) - face_vertices.row(0);
		Eigen::Vector2d v2_2d = face_vertices.row(2) - face_vertices.row(0);
		Eigen::Vector3d v1_3d = Eigen::Vector3d(v1_2d.x(), v1_2d.y(), 0);
		Eigen::Vector3d v2_3d = Eigen::Vector3d(v2_2d.x(), v2_2d.y(), 0);
		Eigen::Vector3d face_normal = v1_3d.cross(v2_3d);

		// If face is flipped (that is, cross-product do not obey the right-hand rule)
		if (face_normal(2) < 0)
		{
			// Reflect the face over the X-axis (so its vertices will be CCW oriented)
			Vs(Fs(i, 0), 1) = -Vs(Fs(i, 0), 1);
			Vs(Fs(i, 1), 1) = -Vs(Fs(i, 1), 1);
			Vs(Fs(i, 2), 1) = -Vs(Fs(i, 2), 1);
		}
	}
}

void MeshWrapper::ComputeSoup2DRandom(const Eigen::MatrixX3i& Fs, Eigen::MatrixX2d& Vs)
{
	auto nvs = Fs.rows() * 3;
	Vs = Eigen::MatrixX2d::Random(nvs, 2) * 2.0;
	FixFlippedFaces(Fs, Vs);
}

void MeshWrapper::ComputeFs(const Eigen::MatrixX3i& F, Eigen::MatrixX3i& Fs)
{
	auto nf = F.rows();
	auto nfs = 3 * nf;
	Eigen::VectorXi lin = Eigen::VectorXi::LinSpaced(nfs, 0, nfs - 1);
	Fs = Eigen::Map<Eigen::Matrix3Xi>(lin.data(), 3, nf).transpose();
}

void MeshWrapper::ComputeE(const Eigen::MatrixX3i& F, Eigen::MatrixX2i& E)
{
	Eigen::SparseMatrix<int> adjacency_matrix;
	igl::adjacency_matrix(F, adjacency_matrix);
	E.resize(adjacency_matrix.nonZeros() >> 1, 2);
	Eigen::MatrixX2i::Index index = 0;
	for (Eigen::SparseMatrix<int>::Index outer_index = 0; outer_index < adjacency_matrix.outerSize(); ++outer_index)
	{
		for (Eigen::SparseMatrix<int>::InnerIterator it(adjacency_matrix, outer_index); it; ++it)
		{
			int row = it.row();
			int column = it.col();
			if (column < row)
			{
				E(index, 0) = row;
				E(index, 1) = column;
				index++;
			}
		}
	}
}

void MeshWrapper::ComputeED2EIMap(const Eigen::MatrixX2i& E, ED2EIMap& ed2ei)
{
	for (Eigen::MatrixX2i::Index row = 0; row < E.rows(); ++row)
	{
		ed2ei[std::make_pair(E(row, 0), E(row, 1))] = row;
	}
}

void MeshWrapper::ComputeVI2VIMaps(const Eigen::MatrixX3i& F, VI2VIsMap& vi2vis, VIs2VIMap& vis2vi)
{
	for (Eigen::MatrixX3i::Index face_index = 0; face_index < F.rows(); ++face_index)
	{
		IndexType base_vertex_index = 3 * face_index;

		// Populate vi2vis
		vi2vis[F(face_index, 0)].push_back(base_vertex_index);
		vi2vis[F(face_index, 1)].push_back(base_vertex_index + 1);
		vi2vis[F(face_index, 2)].push_back(base_vertex_index + 2);

		// Populate vis2vi
		vis2vi[base_vertex_index] = F(face_index, 0);
		vis2vi[base_vertex_index + 1] = F(face_index, 1);
		vis2vi[base_vertex_index + 2] = F(face_index, 2);
	}
}

void MeshWrapper::ComputeEI2EIMaps(const Eigen::MatrixX2i& Es, const VIs2VIMap& vis2vi, const ED2EIMap& ed2ei, EI2EIsMap& ei2eis, EIs2EIMap& eis2ei)
{
	for (Eigen::MatrixX2i::Index edge_index_s = 0; edge_index_s < Es.rows(); ++edge_index_s)
	{
		auto v1_index = Es(edge_index_s, 0);
		auto v2_index = Es(edge_index_s, 1);
		auto v1s_index = vis2vi.at(v1_index);
		auto v2s_index = vis2vi.at(v2_index);
		auto edge_index = ed2ei.at(std::make_pair(v1s_index, v2s_index));
		ei2eis[edge_index].push_back(edge_index_s);
		eis2ei[edge_index_s] = edge_index;
	}
}

void MeshWrapper::ComputeCC(const Eigen::MatrixX2i& E, const Eigen::MatrixX2i& Es, const EI2EIsMap& ei2eis, const VIs2VIMap& vis2vi, Eigen::SparseMatrix<double>& CC)
{
	IndexType current_triplet_index = 0;
	std::vector<Eigen::Triplet<double>> triplets;

	// Iterate over each edge of the original model
	for (Eigen::MatrixX2i::Index edge_index = 0; edge_index < E.rows(); ++edge_index)
	{
		// Get a vector of the edge's soup copies (indices)
		std::vector<IndexType> soup_edge_indices = ei2eis.at(edge_index);

		// If the edge has two soup copies
		if (soup_edge_indices.size() == 2)
		{
			IndexType edge1_index_s = soup_edge_indices[0];
			IndexType edge2_index_s = soup_edge_indices[1];

			IndexType e1_v1_index_s = Es(edge1_index_s, 0);
			IndexType e1_v2_index_s = Es(edge1_index_s, 1);
			IndexType e2_v1_index_s = Es(edge2_index_s, 0);
			IndexType e2_v2_index_s = Es(edge2_index_s, 1);

			IndexType e1_v1_index = vis2vi.at(e1_v1_index_s);
			IndexType e1_v2_index = vis2vi.at(e1_v2_index_s);
			IndexType e2_v1_index = vis2vi.at(e2_v1_index_s);
			IndexType e2_v2_index = vis2vi.at(e2_v2_index_s);

			std::unordered_map<IndexType, std::vector<IndexType>> model_index_to_soup_indices;
			model_index_to_soup_indices[e1_v1_index].push_back(e1_v1_index_s);
			model_index_to_soup_indices[e1_v2_index].push_back(e1_v2_index_s);
			model_index_to_soup_indices[e2_v1_index].push_back(e2_v1_index_s);
			model_index_to_soup_indices[e2_v2_index].push_back(e2_v2_index_s);

			for (auto& it : model_index_to_soup_indices)
			{
				std::pair<IndexType, IndexType> cc = std::minmax(it.second[0], it.second[1]);
				triplets.push_back(Eigen::Triplet<double>(current_triplet_index, cc.first, 1));
				triplets.push_back(Eigen::Triplet<double>(current_triplet_index, cc.second, -1));
				current_triplet_index++;
			}
		}
	}

	CC.resize(triplets.size() >> 1, vis2vi.size());
	CC.setFromTriplets(triplets.begin(), triplets.end());
}

void MeshWrapper::ComputeSurfaceGradientPerFace(const Eigen::MatrixX3d& V, const Eigen::MatrixX3i& F, Eigen::MatrixX3d& D1, Eigen::MatrixX3d& D2)
{
	Eigen::MatrixX3d F1, F2, F3;
	igl::local_basis(V, F, F1, F2, F3);

	const int Fn = F.rows();
	const int vn = V.rows();

	Eigen::MatrixXd Dx(Fn, 3);
	Eigen::MatrixXd Dy(Fn, 3);
	Eigen::MatrixXd Dz(Fn, 3);
	Eigen::MatrixXd fN;
	Eigen::VectorXd Ar;

	igl::per_face_normals(V, F, fN);
	igl::doublearea(V, F, Ar);

	Eigen::Vector3i Pi;
	Pi << 1, 2, 0;
	Eigen::PermutationMatrix<3> P = Eigen::PermutationMatrix<3>(Pi);

	for (int i = 0; i < Fn; i++)
	{
		// renaming indices of vertices of triangles for convenience
		int i1 = F(i, 0);
		int i2 = F(i, 1);
		int i3 = F(i, 2);

		// #F x 3 matrices of triangle edge vectors, named after opposite vertices
		Eigen::Matrix3d e;
		e.col(0) = V.row(i2) - V.row(i1);
		e.col(1) = V.row(i3) - V.row(i2);
		e.col(2) = V.row(i1) - V.row(i3);;

		Eigen::Vector3d Fni = fN.row(i);
		double Ari = Ar(i);

		Eigen::Matrix3d n_M;
		n_M << 0, -Fni(2), Fni(1), Fni(2), 0, -Fni(0), -Fni(1), Fni(0), 0;
		Eigen::VectorXi R(3); R << 0, 1, 2;
		Eigen::VectorXi C(3); C << 3 * i + 2, 3 * i, 3 * i + 1;
		Eigen::Matrix3d res = ((1. / Ari) * (n_M * e)) * P;

		Dx.row(i) = res.row(0);
		Dy.row(i) = res.row(1);
		Dz.row(i) = res.row(2);
	}

	D1 = F1.col(0).asDiagonal() * Dx + F1.col(1).asDiagonal() * Dy + F1.col(2).asDiagonal() * Dz;
	D2 = F2.col(0).asDiagonal() * Dx + F2.col(1).asDiagonal() * Dy + F2.col(2).asDiagonal() * Dz;
}

void MeshWrapper::NormalizeMesh(Eigen::MatrixX3d& V)
{
	Eigen::RowVector3d barycenter = (V.colwise().minCoeff() + V.colwise().maxCoeff()) / 2.0;
	V = V.rowwise() - barycenter;
	double max_coeff = V.cwiseAbs().maxCoeff();
	V = V / max_coeff;
}