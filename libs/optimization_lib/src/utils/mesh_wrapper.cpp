// STL includes
//#include <ranges>

// Optimization library includes
#include "utils/mesh_wrapper.h"

// LIBIGL includes
#include <igl/slice.h>
#include <igl/local_basis.h>
#include <igl/doublearea.h>
#include <igl/per_face_normals.h>
#include <igl/adjacency_matrix.h>

MeshWrapper::MeshWrapper()
{

}

MeshWrapper::MeshWrapper(const Eigen::MatrixX3d& v, const Eigen::MatrixX3i& f) :
	v_dom_(v),
	f_dom_(f)
{
	NormalizeVertices(v_dom_);
	GenerateRandom2DSoup(f_dom_, f_im_, v_im_);
	ComputeEdges(f_dom_, e_dom_);
	ComputeEdges(f_im_, e_im_);
	ComputeED2EIMap(e_dom_, ed_dom_2_ei_dom_);
	ComputeSoupVI2VIMap(f_dom_, v_im_2_v_dom_);
	ComputeVI2SoupVIsMap(f_dom_, v_dom_2_v_im_);
	ComputeSoupEI2EIMap(e_im_, v_im_2_v_dom_, ed_dom_2_ei_dom_, e_im_2_e_dom_);
	ComputeEI2SoupEIsMap(e_im_, v_im_2_v_dom_, ed_dom_2_ei_dom_, e_dom_2_e_im_);
	ComputeCornerCorrespondences(e_dom_, e_im_, e_dom_2_e_im_, v_im_2_v_dom_, cc_);
	ComputeSurfaceGradientPerFace(v_dom_, f_dom_, d1_, d2_);
}

MeshWrapper::~MeshWrapper()
{

}

const Eigen::MatrixX3d& MeshWrapper::GetDomainVertices() const
{
	return v_dom_;
}

const Eigen::MatrixX3i& MeshWrapper::GetDomainFaces() const
{
	return f_dom_;
}

const Eigen::MatrixX2i& MeshWrapper::GetDomainEdges() const
{
	return e_dom_;
}

const Eigen::MatrixX2d& MeshWrapper::GetImageVertices() const
{
	return v_im_;
}

const Eigen::MatrixX3i& MeshWrapper::GetImageFaces() const
{
	return f_im_;
}

const Eigen::MatrixX2i& MeshWrapper::GetImageEdges() const
{
	return e_im_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD1() const
{
	return d1_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD2() const
{
	return d2_;
}

void MeshWrapper::GenerateSoupFaces(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out)
{
	auto f_in_count = f_in.rows();
	auto f_out_count = 3 * f_in_count;
	Eigen::VectorXi lin = Eigen::VectorXi::LinSpaced(f_out_count, 0, f_out_count - 1);
	f_out = Eigen::Map<Eigen::Matrix3Xi>(lin.data(), 3, f_in_count).transpose();
}

void MeshWrapper::FixFlippedFaces(const Eigen::MatrixX3i& f_im, Eigen::MatrixX2d& v_im)
{
	Eigen::Matrix<double, 3, 2> face_vertices;
	for (Eigen::MatrixX3i::Index i = 0; i < f_im.rows(); ++i)
	{
		igl::slice(v_im, f_im.row(i), 1, face_vertices);
		Eigen::Vector2d v1_2d = face_vertices.row(1) - face_vertices.row(0);
		Eigen::Vector2d v2_2d = face_vertices.row(2) - face_vertices.row(0);
		Eigen::Vector3d v1_3d = Eigen::Vector3d(v1_2d.x(), v1_2d.y(), 0);
		Eigen::Vector3d v2_3d = Eigen::Vector3d(v2_2d.x(), v2_2d.y(), 0);
		Eigen::Vector3d face_normal = v1_3d.cross(v2_3d);

		// If face is flipped (that is, cross-product do not obey the right-hand rule)
		if (face_normal(2) < 0)
		{
			// Reflect the face over the X-axis (so its vertices will be CCW oriented)
			v_im(f_im(i, 0), 1) = -v_im(f_im(i, 0), 1);
			v_im(f_im(i, 1), 1) = -v_im(f_im(i, 1), 1);
			v_im(f_im(i, 2), 1) = -v_im(f_im(i, 2), 1);
		}
	}
}

void MeshWrapper::GenerateRandom2DSoup(const Eigen::MatrixX3i& f_in, Eigen::MatrixX3i& f_out, Eigen::MatrixX2d& v_out)
{
	GenerateSoupFaces(f_in, f_out);
	v_out = Eigen::MatrixX2d::Random(3 * f_out.rows(), 2) * 2.0;
	FixFlippedFaces(f_out, v_out);
}

void MeshWrapper::ComputeEdges(const Eigen::MatrixX3i& f, Eigen::MatrixX2i& e)
{
	Eigen::SparseMatrix<int> adjacency_matrix;
	igl::adjacency_matrix(f, adjacency_matrix);
	e.resize(adjacency_matrix.nonZeros() >> 1, 2);
	Eigen::MatrixX2i::Index index = 0;
	for (Eigen::SparseMatrix<int>::Index outer_index = 0; outer_index < adjacency_matrix.outerSize(); ++outer_index)
	{
		for (Eigen::SparseMatrix<int>::InnerIterator it(adjacency_matrix, outer_index); it; ++it)
		{
			int row = it.row();
			int column = it.col();
			if (column < row)
			{
				e(index, 0) = row;
				e(index, 1) = column;
				index++;
			}
		}
	}
}

void MeshWrapper::ComputeSurfaceGradientPerFace(const Eigen::MatrixX3d& v, const Eigen::MatrixX3i& f, Eigen::MatrixX3d& d1, Eigen::MatrixX3d& d2)
{
	Eigen::MatrixX3d F1, F2, F3;
	igl::local_basis(v, f, F1, F2, F3);

	auto f_count = f.rows();
	auto v_count = v.rows();

	Eigen::MatrixXd Dx(f_count, 3);
	Eigen::MatrixXd Dy(f_count, 3);
	Eigen::MatrixXd Dz(f_count, 3);
	Eigen::MatrixXd fN;
	Eigen::VectorXd Ar;

	igl::per_face_normals(v, f, fN);
	igl::doublearea(v, f, Ar);

	Eigen::Vector3i Pi;
	Pi << 1, 2, 0;
	Eigen::PermutationMatrix<3> P = Eigen::PermutationMatrix<3>(Pi);

	for (int i = 0; i < f_count; i++)
	{
		// renaming indices of vertices of triangles for convenience
		int i1 = f(i, 0);
		int i2 = f(i, 1);
		int i3 = f(i, 2);

		// #F x 3 matrices of triangle edge vectors, named after opposite vertices
		Eigen::Matrix3d e;
		e.col(0) = v.row(i2) - v.row(i1);
		e.col(1) = v.row(i3) - v.row(i2);
		e.col(2) = v.row(i1) - v.row(i3);;

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

	d1 = F1.col(0).asDiagonal() * Dx + F1.col(1).asDiagonal() * Dy + F1.col(2).asDiagonal() * Dz;
	d2 = F2.col(0).asDiagonal() * Dx + F2.col(1).asDiagonal() * Dy + F2.col(2).asDiagonal() * Dz;
}

void MeshWrapper::NormalizeVertices(Eigen::MatrixX3d& v)
{
	Eigen::RowVector3d barycenter = (v.colwise().minCoeff() + v.colwise().maxCoeff()) / 2.0;
	v = v.rowwise() - barycenter;
	double max_coeff = v.cwiseAbs().maxCoeff();
	v = v / max_coeff;
}

void MeshWrapper::ComputeED2EIMap(const Eigen::MatrixX2i& E, ED2EIMap& ed2ei)
{
	for (Eigen::MatrixX2i::Index row = 0; row < E.rows(); ++row)
	{
		ed2ei[std::make_pair(E(row, 0), E(row, 1))] = row;
	}
}

void MeshWrapper::IterateSoupFaces(const Eigen::MatrixX3i& f_dom, std::function<void(Eigen::MatrixX3i::Index, IndexType)> callback)
{
	for (Eigen::MatrixX3i::Index face_index = 0; face_index < f_dom.rows(); ++face_index)
	{
		IndexType base_vertex_index = 3 * face_index;
		callback(face_index, base_vertex_index);
	}
}

void MeshWrapper::IterateSoupEdges(const Eigen::MatrixX2i& e_im, const VI2VIMap& v_im_2_v_dom, const ED2EIMap& ed_dom_2_ei_dom, std::function<void(IndexType, IndexType)> callback)
{
	for (Eigen::MatrixX2i::Index edge_index_im = 0; edge_index_im < e_im.rows(); ++edge_index_im)
	{
		auto v1_index_dom = e_im(edge_index_im, 0);
		auto v2_index_dom = e_im(edge_index_im, 1);
		auto v1_index_im = v_im_2_v_dom.at(v1_index_dom);
		auto v2_index_im = v_im_2_v_dom.at(v2_index_dom);
		auto edge_index_dom = ed_dom_2_ei_dom.at(std::make_pair(v1_index_im, v2_index_im));
		callback(edge_index_dom, edge_index_im);
	}
}

void MeshWrapper::ComputeSoupVI2VIMap(const Eigen::MatrixX3i& f_dom, VI2VIMap& v_im_2_v_dom)
{
	IterateSoupFaces(f_dom, [&](Eigen::MatrixX3i::Index face_index, IndexType base_vertex_index) {
		v_im_2_v_dom[base_vertex_index] = f_dom(face_index, 0);
		v_im_2_v_dom[base_vertex_index + 1] = f_dom(face_index, 1);
		v_im_2_v_dom[base_vertex_index + 2] = f_dom(face_index, 2);
	});
}

void MeshWrapper::ComputeVI2SoupVIsMap(const Eigen::MatrixX3i& f_dom, VI2VIsMap& v_dom_2_v_im)
{
	IterateSoupFaces(f_dom, [&](Eigen::MatrixX3i::Index face_index, IndexType base_vertex_index) {
		v_dom_2_v_im[f_dom(face_index, 0)].push_back(base_vertex_index);
		v_dom_2_v_im[f_dom(face_index, 1)].push_back(base_vertex_index + 1);
		v_dom_2_v_im[f_dom(face_index, 2)].push_back(base_vertex_index + 2);
	});
}

void MeshWrapper::ComputeSoupEI2EIMap(const Eigen::MatrixX2i& e_im, const VI2VIMap& v_im_2_v_dom, const ED2EIMap& ed_dom_2_ei_dom, EI2EIMap& e_im_2_e_dom)
{
	IterateSoupEdges(e_im, v_im_2_v_dom, ed_dom_2_ei_dom, [&](IndexType edge_index_dom, IndexType edge_index_im) {
		e_im_2_e_dom[edge_index_im] = edge_index_dom;
	});
}

void MeshWrapper::ComputeEI2SoupEIsMap(const Eigen::MatrixX2i& e_im, const VI2VIMap& v_im_2_v_dom, const ED2EIMap& ed_dom_2_ei_dom, EI2EIsMap& e_dom_2_e_im)
{
	IterateSoupEdges(e_im, v_im_2_v_dom, ed_dom_2_ei_dom, [&](IndexType edge_index_dom, IndexType edge_index_im) {
		e_dom_2_e_im[edge_index_dom].push_back(edge_index_im);
	});
}

void MeshWrapper::ComputeCornerCorrespondences(const Eigen::MatrixX2i& e_dom, const Eigen::MatrixX2i& e_im, const EI2EIsMap& e_dom_2_e_im, const VI2VIMap& v_im_2_v_dom, Eigen::SparseMatrix<double>& cc)
{
	IndexType current_triplet_index = 0;
	std::vector<Eigen::Triplet<double>> triplets;

	// Iterate over each edge of the original model
	for (Eigen::MatrixX2i::Index edge_index = 0; edge_index < e_dom.rows(); ++edge_index)
	{
		// Get a vector of the edge's soup copies (indices)
		std::vector<IndexType> soup_edge_indices = e_dom_2_e_im.at(edge_index);

		// If the edge has two soup copies
		if (soup_edge_indices.size() == 2)
		{
			IndexType edge1_index_im = soup_edge_indices[0];
			IndexType edge2_index_im = soup_edge_indices[1];

			IndexType e1_v1_index_im = e_im(edge1_index_im, 0);
			IndexType e1_v2_index_im = e_im(edge1_index_im, 1);
			IndexType e2_v1_index_im = e_im(edge2_index_im, 0);
			IndexType e2_v2_index_im = e_im(edge2_index_im, 1);

			IndexType e1_v1_index_dom = v_im_2_v_dom.at(e1_v1_index_im);
			IndexType e1_v2_index_dom = v_im_2_v_dom.at(e1_v2_index_im);
			IndexType e2_v1_index_dom = v_im_2_v_dom.at(e2_v1_index_im);
			IndexType e2_v2_index_dom = v_im_2_v_dom.at(e2_v2_index_im);

			std::unordered_map<IndexType, std::vector<IndexType>> dom_index_2_im_indices;
			dom_index_2_im_indices[e1_v1_index_dom].push_back(e1_v1_index_im);
			dom_index_2_im_indices[e1_v2_index_dom].push_back(e1_v2_index_im);
			dom_index_2_im_indices[e2_v1_index_dom].push_back(e2_v1_index_im);
			dom_index_2_im_indices[e2_v2_index_dom].push_back(e2_v2_index_im);

			for (auto& it : dom_index_2_im_indices)
			{
				std::pair<IndexType, IndexType> cc = std::minmax(it.second[0], it.second[1]);
				triplets.push_back(Eigen::Triplet<double>(current_triplet_index, cc.first, 1));
				triplets.push_back(Eigen::Triplet<double>(current_triplet_index, cc.second, -1));
				current_triplet_index++;
			}
		}
	}

	cc.resize(triplets.size() >> 1, v_im_2_v_dom.size());
	cc.setFromTriplets(triplets.begin(), triplets.end());
}

void AutocutsEngine::FindEdgeLenghtsForSeparation()
{
	SpMat Esept = solverWrapper->solver->energy->separation->Esept;
	Vec edge_lengths = Vec::Ones(Esept.cols());
	int v1, v2, num_edges, pair1, pair2;
	double edge_len;
	Vec3 v1pos, v2pos;
	double total = 0.;
	int cnt = 0;
	for (int i = 0; i < modelFacesMatrix.rows(); ++i)
	{
		Vec3i face = modelFacesMatrix.row(i);
		for (int j = 0; j < 3; ++j)
		{ // loop over all 3 triangle edges
			v1 = face(j);
			v2 = face((j + 1) % 3);
			num_edges = FindCorrespondingUvEdges(v1, v2);
			if (num_edges == 2)
			{ // this is an actual separation energy pair
				v1pos = modelVerticesMatrix.row(v1);
				v2pos = modelVerticesMatrix.row(v2);
				edge_len = 0.5 * (v1pos - v2pos).squaredNorm();
				total += (v1pos - v2pos).norm();
				cnt++;
				pair1 = FindCorrespondingPairIndex(uvEdges[0].first, uvEdges[1].first);
				pair2 = FindCorrespondingPairIndex(uvEdges[0].second, uvEdges[1].second);
				if (pair1 == -1 || pair2 == -1)
				{
					pair1 = FindCorrespondingPairIndex(uvEdges[0].first, uvEdges[1].second);
					pair2 = FindCorrespondingPairIndex(uvEdges[0].second, uvEdges[1].first);
				}
				edge_lengths(pair1) = edge_len;
				edge_lengths(pair2) = edge_len;
			}
		}
	}

	if (setEdgeLenghtsToAverage)
	{
		edge_lengths = Vec::Constant(Esept.cols(), total / (double)cnt);
	}

	solverWrapper->get_slot();
	solverWrapper->solver->energy->separation->edge_lenghts_per_pair = edge_lengths;
	solverWrapper->release_slot();
}