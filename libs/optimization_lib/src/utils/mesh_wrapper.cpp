// STL includes
//#include <ranges>

// Optimization library includes
#include <utils/mesh_wrapper.h>

// LIBIGL includes
#include <igl/slice.h>
#include <igl/local_basis.h>
#include <igl/doublearea.h>
#include <igl/per_face_normals.h>
#include <igl/adjacency_matrix.h>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

MeshWrapper::MeshWrapper()
{

}

MeshWrapper::MeshWrapper(const Eigen::MatrixX3d& v, const Eigen::MatrixX3i& f) :
	v_dom_(v),
	f_dom_(f)
{
	Initialize();
}

MeshWrapper::MeshWrapper(const std::string& modelFilePath)
{
	LoadModel(modelFilePath);
}

MeshWrapper::~MeshWrapper()
{

}

void MeshWrapper::SetImageVertices(const Eigen::MatrixX2d& v_im)
{
	v_im_ = v_im;
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

const MeshWrapper::VI2VIsMap& MeshWrapper::GetImageNeighbours() const
{
	return v_im_2_neighbours;
}

const MeshWrapper::VI2VIsMap& MeshWrapper::GetDomainVerticesToImageVerticesMap() const
{
	return v_dom_2_v_im_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD1() const
{
	return d1_;
}

const Eigen::MatrixX3d& MeshWrapper::GetD2() const
{
	return d2_;
}

const Eigen::SparseMatrix<double>& MeshWrapper::GetCorrespondingVertexPairsCoefficients() const
{
	return cv_pairs_coefficients_;
}

const Eigen::VectorXd& MeshWrapper::GetCorrespondingVertexPairsEdgeLength() const
{
	return cv_pairs_edge_length_;
}

void MeshWrapper::LoadModel(const std::string& model_file_path)
{
	/**
	 * Get file type
	 */
	MeshWrapper::ModelFileType modelFileType = GetModelFileType(model_file_path);
	if (modelFileType == MeshWrapper::ModelFileType::UNKNOWN)
	{
		return;
	}

	/**
	 * Read file
	 */
	Eigen::MatrixXd v;
	Eigen::MatrixXi f;
	switch (modelFileType)
	{
	case MeshWrapper::ModelFileType::OFF:
		igl::readOFF(model_file_path, v, f);
		break;
	case MeshWrapper::ModelFileType::OBJ:
		igl::readOBJ(model_file_path, v, f);
		break;
	}

	/**
	 * Triangulate (if faces were received as quads)
	 */
	if (f.cols() == 4)
	{
		auto f_triangulated = Eigen::MatrixXi(f.rows() * 2, 3);
		for (Eigen::DenseIndex i = 0; i < f.rows(); ++i)
		{
			auto face = f.row(i);
			auto triangle_index = 2 * i;
			f_triangulated.row(triangle_index) << face[0], face[1], face[3];
			f_triangulated.row(triangle_index + 1) << face[1], face[2], face[3];
		}

		f = f_triangulated;
	}

	v_dom_ = v;
	f_dom_ = f;

	Initialize();

	model_loaded_signal_();
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

		// If face is flipped (that is, cross-product doesn't obey the right-hand rule)
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
	srand((unsigned int)time(0));
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

void MeshWrapper::ComputeEdgeDescriptorMap(const Eigen::MatrixX2i& e, ED2EIMap& ed_2_ei)
{
	for (int64_t edge_index = 0; edge_index < e.rows(); ++edge_index)
	{
		ed_2_ei[std::make_pair(e(edge_index, 0), e(edge_index, 1))] = edge_index;
	}
}

void MeshWrapper::ComputeEdgeIndexMaps()
{
	for (int64_t edge_index_im = 0; edge_index_im < e_im_.rows(); ++edge_index_im)
	{
		auto v1_index_im = e_im_(edge_index_im, 0);
		auto v2_index_im = e_im_(edge_index_im, 1);
		auto v1_index_dom = v_im_2_v_dom_.at(v1_index_im);
		auto v2_index_dom = v_im_2_v_dom_.at(v2_index_im);
		auto edge_index_dom = ed_dom_2_ei_dom_.at(std::make_pair(v1_index_dom, v2_index_dom));

		e_im_2_e_dom_[edge_index_im] = edge_index_dom;
		e_dom_2_e_im_[edge_index_dom].push_back(edge_index_im);
	}
}

void MeshWrapper::ComputeVertexIndexMaps()
{
	for (int64_t face_index = 0; face_index < f_dom_.rows(); ++face_index)
	{
		auto current_face_dom = f_dom_.row(face_index);
		auto current_face_im = f_im_.row(face_index);

		v_im_2_v_dom_[current_face_im(0)] = current_face_dom(0);
		v_im_2_v_dom_[current_face_im(1)] = current_face_dom(1);
		v_im_2_v_dom_[current_face_im(2)] = current_face_dom(2);

		v_dom_2_v_im_[current_face_dom(0)].push_back(current_face_im(0));
		v_dom_2_v_im_[current_face_dom(1)].push_back(current_face_im(1));
		v_dom_2_v_im_[current_face_dom(2)].push_back(current_face_im(2));
	}
}

void MeshWrapper::ComputeVertexToEdgeIndexMaps()
{
	for (int64_t edge_index = 0; edge_index < e_im_.rows(); ++edge_index)
	{
		auto current_edge_im = e_im_.row(edge_index);

		v_im_2_e_im[current_edge_im(0)].push_back(edge_index);
		v_im_2_e_im[current_edge_im(1)].push_back(edge_index);
	}
}

void MeshWrapper::ComputeCorrespondingPairs()
{
	int64_t current_triplet_index = 0;
	std::vector<Eigen::Triplet<double>> triplets;

	// Iterate over each edge of in the domain
	for (int64_t edge_index_dom = 0; edge_index_dom < e_dom_.rows(); ++edge_index_dom)
	{
		// Get the corresponding copies in the image
		std::vector<int64_t> edge_indices_im = e_dom_2_e_im_.at(edge_index_dom);

		// If the domain edge has two image copies
		if (edge_indices_im.size() == 2)
		{
			// Get the indices of the two image edges
			int64_t edge1_index_im = edge_indices_im[0];
			int64_t edge2_index_im = edge_indices_im[1];

			// Get the indices of the four image vertices that constitute the two image edges
			int64_t e1_v1_index_im = e_im_(edge1_index_im, 0);
			int64_t e1_v2_index_im = e_im_(edge1_index_im, 1);
			int64_t e2_v1_index_im = e_im_(edge2_index_im, 0);
			int64_t e2_v2_index_im = e_im_(edge2_index_im, 1);

			// Get the indices of the four corresponding domain vertices
			int64_t e1_v1_index_dom = v_im_2_v_dom_.at(e1_v1_index_im);
			int64_t e1_v2_index_dom = v_im_2_v_dom_.at(e1_v2_index_im);
			int64_t e2_v1_index_dom = v_im_2_v_dom_.at(e2_v1_index_im);
			int64_t e2_v2_index_dom = v_im_2_v_dom_.at(e2_v2_index_im);

			// Sort the four image vertex-indices according to their corresponding domain vertex-index
			std::unordered_map<int64_t, std::vector<std::pair<int64_t, int64_t>>> dom_v_index_2_im_v_e_indices;
			dom_v_index_2_im_v_e_indices[e1_v1_index_dom].push_back(std::make_pair(e1_v1_index_im, edge1_index_im));
			dom_v_index_2_im_v_e_indices[e1_v2_index_dom].push_back(std::make_pair(e1_v2_index_im, edge1_index_im));
			dom_v_index_2_im_v_e_indices[e2_v1_index_dom].push_back(std::make_pair(e2_v1_index_im, edge2_index_im));
			dom_v_index_2_im_v_e_indices[e2_v2_index_dom].push_back(std::make_pair(e2_v2_index_im, edge2_index_im));

			// Get a liat of unique dom vertices indices values
			std::vector<int64_t> dom_v_indices = { e1_v1_index_dom, e1_v2_index_dom, e2_v1_index_dom, e2_v2_index_dom };
			const auto it = std::unique(dom_v_indices.begin(), dom_v_indices.end());
			dom_v_indices.resize(std::distance(dom_v_indices.begin(), it));

			// Sort each bucket of 'dom_v_index_2_im_v_e_indices' according to the image edge index
			for (auto i = 0; i < dom_v_indices.size(); i++)
			{
				std::sort(dom_v_index_2_im_v_e_indices[dom_v_indices[i]].begin(), dom_v_index_2_im_v_e_indices[dom_v_indices[i]].end(), [](std::pair<int64_t, int64_t>& pair1, std::pair<int64_t, int64_t>& pair2) {
					return pair1.second < pair2.second;
				});
			}

			// Record the two corresponding image vertices pairs
			for (auto& it : dom_v_index_2_im_v_e_indices)
			{
				std::pair<int64_t, int64_t> cv_pair = std::minmax(it.second[0].first, it.second[1].first);
				cv_pairs_.push_back(cv_pair);
			}

			// Get the vertices indices of the image edges in order. that means that 'e1_v1_index_im_ordered' and 'e2_v1_index_im_ordered' both correspond to the same dom vertex, same for the 'v2' indices.
			auto e1_v1_index_im_ordered = dom_v_index_2_im_v_e_indices[dom_v_indices[0]][0].first;
			auto e2_v1_index_im_ordered = dom_v_index_2_im_v_e_indices[dom_v_indices[0]][1].first;
			auto e1_v2_index_im_ordered = dom_v_index_2_im_v_e_indices[dom_v_indices[1]][0].first;
			auto e2_v2_index_im_ordered = dom_v_index_2_im_v_e_indices[dom_v_indices[1]][1].first;

			// Record corresponding image edges, expressed using their vertices
			auto cev_pair = std::make_pair(std::make_pair(e1_v1_index_im_ordered, e1_v2_index_im_ordered), std::make_pair(e2_v1_index_im_ordered, e2_v2_index_im_ordered));
 			cev_pairs_.push_back(cev_pair);
			
			// Record corresponding image edges pair 
			std::pair<int64_t, int64_t> ce_pair = std::minmax(edge1_index_im, edge2_index_im);
			ce_pairs_.push_back(ce_pair);
		}
	}
}

void MeshWrapper::ComputeCorrespondingVertexPairsCoefficients()
{
	std::vector<Eigen::Triplet<double>> triplets;

	for (size_t cv_pair_index = 0; cv_pair_index < cv_pairs_.size(); cv_pair_index++)
	{
		auto cv_pair = cv_pairs_[cv_pair_index];
		triplets.push_back(Eigen::Triplet<double>(cv_pair_index, cv_pair.first, 1));
		triplets.push_back(Eigen::Triplet<double>(cv_pair_index, cv_pair.second, -1));
	}

	cv_pairs_coefficients_.resize(cv_pairs_.size(), v_im_.rows());
	cv_pairs_coefficients_.setFromTriplets(triplets.begin(), triplets.end());
}

void MeshWrapper::ComputeCorrespondingVertexPairsEdgeLength()
{
	auto cv_pairs_size = cv_pairs_.size();
	cv_pairs_edge_length_.resize(cv_pairs_size, 1);
	for (size_t cv_pair_index = 0; cv_pair_index < cv_pairs_size; cv_pair_index++)
	{
		auto cv_pair = cv_pairs_[cv_pair_index];
		auto edge_index_im = ed_im_2_ei_im_[std::make_pair(cv_pair.first, cv_pair.second)];
		auto edge_index_dom = e_im_2_e_dom_[edge_index_im];
		auto v1_index_dom = e_dom_(edge_index_dom, 0);
		auto v2_index_dom = e_dom_(edge_index_dom, 1);
		Eigen::Vector3d v1_dom = v_dom_.row(v1_index_dom);
		Eigen::Vector3d v2_dom = v_dom_.row(v2_index_dom);
		auto edge_length = 0.5 * (v1_dom - v2_dom).squaredNorm();
		cv_pairs_edge_length_(cv_pair_index) = edge_length;
	}
}

void MeshWrapper::ComputeVertexNeighbours()
{
	for (int64_t i = 0; i < v_im_.rows(); i++)
	{
		for (auto& edge_index : v_im_2_e_im[i])
		{
			auto edge = e_im_.row(edge_index);
			for (int64_t col = 0; col < edge.cols(); col++)
			{
				auto vertex_index = edge.coeffRef(col);
				if (vertex_index != i)
				{
					v_im_2_neighbours[i].push_back(vertex_index);
				}				
			}
		}
	}
}

int64_t MeshWrapper::GetImageVerticesCount() const
{
	return v_im_.rows();
}

int64_t MeshWrapper::GetDomainVerticesCount() const
{
	return v_dom_.rows();
}

Eigen::VectorXi MeshWrapper::GetImageFaceVerticesIndices(int64_t face_index)
{
	return f_im_.row(face_index);
}

Eigen::MatrixXd MeshWrapper::GetImageVertices(const Eigen::VectorXi& vertex_indices)
{
	return igl::slice(v_im_, vertex_indices, 1);
}

MeshWrapper::ModelFileType MeshWrapper::GetModelFileType(const std::string& modelFilePath)
{
	std::string fileExtension = modelFilePath.substr(modelFilePath.find_last_of(".") + 1);
	transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
	if (fileExtension == "obj")
	{
		return ModelFileType::OBJ;
	}
	else if (fileExtension == "off")
	{
		return ModelFileType::OFF;
	}

	return ModelFileType::UNKNOWN;
}

void MeshWrapper::Initialize()
{
	NormalizeVertices(v_dom_);
	GenerateRandom2DSoup(f_dom_, f_im_, v_im_);

	ComputeEdges(f_dom_, e_dom_);
	ComputeEdges(f_im_, e_im_);

	ComputeEdgeDescriptorMap(e_dom_, ed_dom_2_ei_dom_);
	ComputeEdgeDescriptorMap(e_im_, ed_im_2_ei_im_);

	ComputeVertexIndexMaps();
	ComputeEdgeIndexMaps();
	ComputeVertexToEdgeIndexMaps();

	ComputeCorrespondingPairs();

	ComputeCorrespondingVertexPairsCoefficients();
	ComputeCorrespondingVertexPairsEdgeLength();

	ComputeVertexNeighbours();

	ComputeSurfaceGradientPerFace(v_dom_, f_dom_, d1_, d2_);
}

void MeshWrapper::RegisterModelLoadedCallback(const std::function<ModelLoadedCallback>& model_loaded_callback)
{
	model_loaded_signal_.connect(model_loaded_callback);
}

const MeshWrapper::EV2EVMap& MeshWrapper::GetCorrespondingEdgeVertices() const
{
	return cev_pairs_;
}