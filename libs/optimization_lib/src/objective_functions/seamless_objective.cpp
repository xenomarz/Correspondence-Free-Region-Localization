#include "CrossEnergy.h"
#include "autodiff.h"
#include "Energies/Seamless/Definitions.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <igl/cat.h>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>  

using namespace std;

double CrossEnergy::EdgePairElement::c0 = 0;
double CrossEnergy::EdgePairElement::c1 = 0;
double CrossEnergy::EdgePairElement::c2 = 4.863416814832212;
double CrossEnergy::EdgePairElement::c3 = -4.128196407449535;

double CrossEnergy::EdgePairElement::c2_mult_2 = c2 * 2;
double CrossEnergy::EdgePairElement::c3_mult_3 = c3 * 3;
double CrossEnergy::EdgePairElement::c3_mult_6 = c3 * 6;

double CrossEnergy::EdgePairElement::pi_1_4 = M_PI / 4;
double CrossEnergy::EdgePairElement::pi_1_2 = M_PI / 2;
double CrossEnergy::EdgePairElement::pi_3_4 = (3 * M_PI) / 4;

CrossEnergy::CrossEnergy() :
	first_update(true)
{
}

CrossEnergy::~CrossEnergy()
{
}

void CrossEnergy::init()
{
	first_update = true;
	edge_pair_elements.clear();

	EdgeToCopies edge_to_copies;
	EdgePairs edge_pairs;
	SpMati E2E = mesh_wrapper->GetE2E();
	for (int i = 0; i < E2E.outerSize(); ++i) {
		for (SpMati::InnerIterator it(E2E, i); it; ++it)
		{
			edge_to_copies[it.col()].push_back(it.row());
		}
	}

	for (EdgeToCopies::iterator it = edge_to_copies.begin(); it != edge_to_copies.end(); it++)
	{
		if (it->second.size() == 2)
		{
			edge_pairs.push_back(EdgePair(it->second[0], it->second[1]));
		}
	}

	SpMati V2Es = mesh_wrapper->GetV2Es();
	for (EdgePairs::iterator edge_pairs_it = edge_pairs.begin(); edge_pairs_it != edge_pairs.end(); edge_pairs_it++)
	{
		vector<int> first_edge_vertices_list;
		vector<int> second_edge_vertices_list;
		for (int i = 0; i < V2Es.outerSize(); ++i) {
			for (SpMati::InnerIterator it(V2Es, i); it; ++it)
			{
				if (edge_pairs_it->first == it.col())
				{
					first_edge_vertices_list.push_back(it.row());
				}
				else if (edge_pairs_it->second == it.col())
				{
					second_edge_vertices_list.push_back(it.row());
				}
			}
		}

		if (first_edge_vertices_list.size() == 2 && second_edge_vertices_list.size() == 2)
		{
			EdgeVertices first_edge_vertices(first_edge_vertices_list[0], first_edge_vertices_list[1]);
			EdgeVertices second_edge_vertices(second_edge_vertices_list[0], second_edge_vertices_list[1]);
			EdgePairVertices edge_pair_vertices = EdgePairVertices(first_edge_vertices, second_edge_vertices);
			edge_pair_elements.push_back(std::make_shared<EdgePairElement>(mesh_wrapper, *edge_pairs_it, edge_pair_vertices));
		}
	}
}

double CrossEnergy::value()
{
	int number_of_soup_vertices = mesh_wrapper->GetNumberOfSoupVertices();
	double accumulated_value = 0;
	vertex_energy = Eigen::VectorXd::Zero(mesh_wrapper->GetNumberOfSoupVertices());

	for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	{
		double value = (*it)->Value();
		accumulated_value += value;
		vertex_energy.coeffRef((*it)->GetEdge1Vertex1()) += value;
		vertex_energy.coeffRef((*it)->GetEdge1Vertex2()) += value;
		vertex_energy.coeffRef((*it)->GetEdge2Vertex1()) += value;
		vertex_energy.coeffRef((*it)->GetEdge2Vertex2()) += value;	
	}

	return accumulated_value;
}

void CrossEnergy::gradient(VectorXd& g)
{
	int number_of_soup_vertices = mesh_wrapper->GetNumberOfSoupVertices();
	MatX2 g_mat = MatX2::Zero(number_of_soup_vertices, 2);

#pragma omp parallel for
	for (int i = 0; i < edge_pair_elements.size(); i++)
	{
		edge_pair_elements[i]->Gradient(g_mat);
	}


	//for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	//{
	//	(*it)->Gradient(g_mat);
	//}
	g = Eigen::Map<Vec>(g_mat.data(), 2.0 * g_mat.rows(), 1);
}

void CrossEnergy::hessian()
{
	//int start_index = 0;
#pragma omp parallel for
	for (int i = 0; i < edge_pair_elements.size(); i++)
	{
		edge_pair_elements[i]->Hessian(SS, i * 36);

		//int my_Size = edge_pair_elements[i]->GetUpperTriangleEntries().size();

		//start_index += edge_pair_elements[i]->GetUpperTriangleEntries().size();
	}

	//for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	//{
	//	(*it)->Hessian(SS, start_index);
	//	start_index += (*it)->GetUpperTriangleEntries().size();
	//}
}

void CrossEnergy::updateX(const VectorXd& X)
{
	Eigen::MatrixX2d X_mat;
	int number_of_soup_vertices = mesh_wrapper->GetNumberOfSoupVertices();
	X_mat.resize(number_of_soup_vertices, Eigen::NoChange);

#pragma omp parallel for
	for (int i = 0; i < number_of_soup_vertices; i++)
	{
		X_mat.row(i) << X(i), X(i + number_of_soup_vertices);
	}

#pragma omp parallel for
	for (int i = 0; i < edge_pair_elements.size(); i++)
	{
		edge_pair_elements[i]->UpdateEdgeVectors(X_mat);
	}

	//for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	//{
	//	(*it)->UpdateEdgeVectors(X_mat);
	//	total_original_length += (*it)->GetOriginalLength();
	//}

	if (first_update)
	{
		double total_original_length = 0;
		for (int i = 0; i < edge_pair_elements.size(); i++)
		{
			total_original_length += edge_pair_elements[i]->GetOriginalLength();
		}

#pragma omp parallel for
		for (int i = 0; i < edge_pair_elements.size(); i++)
		{
			edge_pair_elements[i]->CalculateRelativeOriginalLength(total_original_length);
		}

		//for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
		//{
		//	(*it)->CalculateRelativeOriginalLength(total_original_length);
		//}
		first_update = false;
	}
}

void CrossEnergy::prepare_hessian()
{
	II.clear();
	JJ.clear();
	int n = mesh_wrapper->GetNumberOfSoupVertices();
	auto PushPair = [&](int i, int j) { II.push_back(i); JJ.push_back(j); };
	for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	{
		std::vector<UpperTriangleEntry>& upper_triangle_entries = (*it)->GetUpperTriangleEntries();
		for (UpperTriangleEntry& upper_triangle_entry : upper_triangle_entries)
		{
			PushPair(upper_triangle_entry.GetRow(), upper_triangle_entry.GetColumn());
		}
	}

	SS = vector<double>(II.size(), 0.0);
}

std::vector<int> CrossEnergy::GetSeamlessHistogram(int buckets)
{
	std::vector<int> histogram(buckets);
	double bucketDelta = (4 * M_PI) / (double)buckets;
	for (EdgePairElements::iterator it = edge_pair_elements.begin(); it != edge_pair_elements.end(); it++)
	{
		double angle = (*it)->GetAtan2Diff() + 2 * M_PI;
		int bucket = int(angle / bucketDelta);
		histogram.at(bucket)++;
	}

	return histogram;
}

inline void CrossEnergy::EdgePairElement::BuildUpperTriangleEntries()
{
	int n = mesh_wrapper->GetNumberOfSoupVertices();

	int edge1_vertex1 = this->GetEdge1Vertex1();
	int edge1_vertex2 = this->GetEdge1Vertex2();
	int edge2_vertex1 = this->GetEdge2Vertex1();
	int edge2_vertex2 = this->GetEdge2Vertex2();

	std::vector<int> indices;
	indices.push_back(edge1_vertex1);
	indices.push_back(edge1_vertex2);
	indices.push_back(edge2_vertex1);
	indices.push_back(edge2_vertex2);
	indices.push_back(edge1_vertex1 + n);
	indices.push_back(edge1_vertex2 + n);
	indices.push_back(edge2_vertex1 + n);
	indices.push_back(edge2_vertex2 + n);

	std::vector<int> partials;
	partials.push_back(SeamlessPartials::X_1_I);
	partials.push_back(SeamlessPartials::X_2_I);
	partials.push_back(SeamlessPartials::X_1_J);
	partials.push_back(SeamlessPartials::X_2_J);
	partials.push_back(SeamlessPartials::Y_1_I);
	partials.push_back(SeamlessPartials::Y_2_I);
	partials.push_back(SeamlessPartials::Y_1_J);
	partials.push_back(SeamlessPartials::Y_2_J);

	for (int i = 0; i < indices.size(); i++)
	{
		for (int j = 0; j < indices.size(); j++)
		{
			if (indices[j] >= indices[i])
			{
				upper_triangle_entries.push_back(UpperTriangleEntry(indices[i], indices[j], partials[i], partials[j]));
			}
		}
	}

	std::vector<int> row_indices;
	std::vector<int> column_indices;
	for (UpperTriangleEntry& upper_triangle_entry : upper_triangle_entries)
	{
		row_indices.push_back(upper_triangle_entry.GetRow());
		column_indices.push_back(upper_triangle_entry.GetColumn());
	}

	sort(row_indices.begin(), row_indices.end());
	row_indices.erase(unique(row_indices.begin(), row_indices.end()), row_indices.end());

	sort(column_indices.begin(), column_indices.end());
	column_indices.erase(unique(column_indices.begin(), column_indices.end()), column_indices.end());

	for (int i = 0; i < row_indices.size(); i++)
	{
		hessian_to_mini_hessian_row[row_indices.at(i)] = i;
		mini_hessian_to_hessian_row[i] = row_indices.at(i);
	}

	for (int i = 0; i < column_indices.size(); i++)
	{
		hessian_to_mini_hessian_column[column_indices.at(i)] = i;
		mini_hessian_to_hessian_column[i] = column_indices.at(i);
	}

	for (UpperTriangleEntry& upper_triangle_entry : upper_triangle_entries)
	{
		mini_hessian_row_coumn_to_upper_triangle_entry[std::pair<int, int>(hessian_to_mini_hessian_row[upper_triangle_entry.GetRow()], hessian_to_mini_hessian_column[upper_triangle_entry.GetColumn()])] = upper_triangle_entry;
	}
}

std::vector<UpperTriangleEntry>& CrossEnergy::EdgePairElement::GetUpperTriangleEntries()
{
	return upper_triangle_entries;
}

CrossEnergy::EdgePairElement::EdgePairElement(shared_ptr<MeshWrapper> mesh_wrapper, EdgePair edge_pair, EdgePairVertices edge_pair_vertices) :
	mesh_wrapper(mesh_wrapper),
	edge_pair(edge_pair),
	edge_pair_vertices(edge_pair_vertices),
	atan2_diff_exp(),
	first_update(true),
	original_length(1),
	relative_original_length(1)
{
	BuildUpperTriangleEntries();
}

int CrossEnergy::EdgePairElement::GetEdge1Vertex1()
{
	return edge_pair_vertices.first.first;
}

int CrossEnergy::EdgePairElement::GetEdge1Vertex2()
{
	return edge_pair_vertices.first.second;
}

int CrossEnergy::EdgePairElement::GetEdge2Vertex1()
{
	return edge_pair_vertices.second.first;
}

int CrossEnergy::EdgePairElement::GetEdge2Vertex2()
{
	return edge_pair_vertices.second.second;
}

int CrossEnergy::EdgePairElement::GetEdge1()
{
	return edge_pair.first;
}

int CrossEnergy::EdgePairElement::GetEdge2()
{
	return edge_pair.second;
}

void CrossEnergy::EdgePairElement::CalculateRelativeOriginalLength(double total_original_length)
{
	relative_original_length = original_length / total_original_length;
}

double CrossEnergy::EdgePairElement::GetOriginalLength()
{
	return original_length;
}

void CrossEnergy::EdgePairElement::UpdateEdgeVectors(const Eigen::MatrixX2d& X)
{
	double x1_i = X.coeff(edge_pair_vertices.first.first, 0);
	double y1_i = X.coeff(edge_pair_vertices.first.first, 1);

	double x2_i = X.coeff(edge_pair_vertices.first.second, 0);
	double y2_i = X.coeff(edge_pair_vertices.first.second, 1);

	double x1_j = X.coeff(edge_pair_vertices.second.first, 0);
	double y1_j = X.coeff(edge_pair_vertices.second.first, 1);

	double x2_j = X.coeff(edge_pair_vertices.second.second, 0);
	double y2_j = X.coeff(edge_pair_vertices.second.second, 1);

	if (first_update)
	{
		first_update = false;
		original_length = std::sqrt(std::pow(x1_i - x2_i, 2) + std::pow(x2_i - y2_i, 2));
	}

	std::vector<double> arguments = vector<double>{ x1_i, x2_i, y1_i, y2_i, x1_j, x2_j, y1_j, y2_j };
	atan2_diff_exp.SetArguments(arguments);

	atan2_diff = atan2_diff_exp.Evaluate();
	atan2_diff_mod = fmod(atan2_diff, pi_1_2);
	if (atan2_diff_mod < 0)
	{
		if (atan2_diff_mod < -pi_1_4)
		{
			atan2_diff_shifted = atan2_diff_mod + pi_1_2;
			sign = 1;
		}
		else
		{
			atan2_diff_shifted = -atan2_diff_mod;
			sign = -1;
		}
	}
	else
	{
		if (atan2_diff_mod > pi_1_4)
		{
			atan2_diff_shifted = pi_1_2 - atan2_diff_mod;
			sign = -1;
		}
		else
		{
			atan2_diff_shifted = atan2_diff_mod;
			sign = 1;
		}
	}

	if (((atan2_diff_mod > pi_1_4) && (atan2_diff_mod < pi_3_4)) || ((atan2_diff_mod < -pi_1_4) && (atan2_diff_mod > -pi_3_4)))
	{
		sign2 = -1;
	}
	else
	{
		sign2 = 1;
	}

	atan2_diff_shifted_squared = atan2_diff_shifted * atan2_diff_shifted;
	atan2_diff_shifted_tripled = atan2_diff_shifted_squared * atan2_diff_shifted;
}

double CrossEnergy::EdgePairElement::Value()
{
	return relative_original_length * (c3 * atan2_diff_shifted_tripled + c2 * atan2_diff_shifted_squared);


	//return relative_original_length * (c3 * pow(atan2_diff_shifted, 3) + c2 * pow(atan2_diff_shifted, 2));

	//return c3 * pow(atan2_diff_shifted, 3) + c2 * pow(atan2_diff_shifted, 2) + c1 * atan2_diff_shifted + c0;
}

inline double CrossEnergy::EdgePairElement::FirstPartialDerivative(int partial)
{
	double d_atan2_diff = atan2_diff_exp.EvaluateFirstPartialDerivative(partial);
	return relative_original_length * (sign * (c3_mult_3 * atan2_diff_shifted_squared * d_atan2_diff + c2_mult_2 * atan2_diff_shifted * d_atan2_diff));

	//double d_atan2_diff = atan2_diff_exp.EvaluateFirstPartialDerivative(partial);
	//return relative_original_length * (sign * (3 * c3 * atan2_diff_shifted_squared * d_atan2_diff + 2 * c2 * atan2_diff_shifted * d_atan2_diff));

	//return sign * (3 * c3 * atan2_diff_shifted_squared * d_atan2 + 2 * c2 * atan2_diff_shifted * d_atan2 + c1 * d_atan2);
}

inline double CrossEnergy::EdgePairElement::SecondPartialDerivative(int first_partial, int second_partial)
{
	double d_atan2_diff_first = atan2_diff_exp.EvaluateFirstPartialDerivative(first_partial);
	double d_atan2_diff_second = atan2_diff_exp.EvaluateFirstPartialDerivative(second_partial);
	double d2_atan2_diff = atan2_diff_exp.EvaluateSecondPartialDerivative(first_partial, second_partial);

	double factor1 = d_atan2_diff_first * sign * d_atan2_diff_second;
	double part1 = (c3_mult_6 * atan2_diff_shifted * factor1) + (c3_mult_3 * atan2_diff_shifted_squared * d2_atan2_diff);
	double part2 = c2_mult_2 * ((factor1) + (atan2_diff_shifted * d2_atan2_diff));


	//double part1 = (6 * c3 * atan2_diff_shifted * d_atan2_diff_first * sign * d_atan2_diff_second) + (3 * c3 * atan2_diff_shifted_squared * d2_atan2_diff);
	//double part2 = (2 * c2 * d_atan2_diff_first * sign * d_atan2_diff_second) + (2 * c2 * atan2_diff_shifted * d2_atan2_diff);

	// (1 - relative_original_length) will be zero when we have only two triangles with 1 shared edge
	//double factor;
	//if ((1 - relative_original_length) < std::numeric_limits<double>::epsilon())
	//{
	//	factor = 1;
	//}
	//else
	//{
	//	factor = (1 / (1 - relative_original_length));
	//}

	return relative_original_length * (sign * (part1 + part2));
}

void CrossEnergy::EdgePairElement::Gradient(MatX2& g_mat)
{
	double df_dx_1_i = FirstPartialDerivative(SeamlessPartials::X_1_I);
	double df_dy_1_i = FirstPartialDerivative(SeamlessPartials::Y_1_I);
	double df_dx_2_i = FirstPartialDerivative(SeamlessPartials::X_2_I);
	double df_dy_2_i = FirstPartialDerivative(SeamlessPartials::Y_2_I);

	double df_dx_1_j = FirstPartialDerivative(SeamlessPartials::X_1_J);
	double df_dy_1_j = FirstPartialDerivative(SeamlessPartials::Y_1_J);
	double df_dx_2_j = FirstPartialDerivative(SeamlessPartials::X_2_J);
	double df_dy_2_j = FirstPartialDerivative(SeamlessPartials::Y_2_J);

	g_mat.coeffRef(edge_pair_vertices.first.first, 0) += df_dx_1_i;
	g_mat.coeffRef(edge_pair_vertices.first.first, 1) += df_dy_1_i;

	g_mat.coeffRef(edge_pair_vertices.first.second, 0) += df_dx_2_i;
	g_mat.coeffRef(edge_pair_vertices.first.second, 1) += df_dy_2_i;

	g_mat.coeffRef(edge_pair_vertices.second.first, 0) += df_dx_1_j;
	g_mat.coeffRef(edge_pair_vertices.second.first, 1) += df_dy_1_j;

	g_mat.coeffRef(edge_pair_vertices.second.second, 0) += df_dx_2_j;
	g_mat.coeffRef(edge_pair_vertices.second.second, 1) += df_dy_2_j;
}

void CrossEnergy::EdgePairElement::Hessian(std::vector<double>& SS, int index)
{
	MatrixXd mini_hessian(8,8);

	for (int row = 0; row < SeamlessPartials::COUNT; row++)
	{
		for (int column = 0; column < SeamlessPartials::COUNT; column++)
		{
			if (column >= row)
			{
				UpperTriangleEntry& upper_triangle_entry = mini_hessian_row_coumn_to_upper_triangle_entry[std::pair<int, int>(row, column)];
				double value = SecondPartialDerivative(upper_triangle_entry.GetRowPartial(), upper_triangle_entry.GetColumnPartial());
				mini_hessian.coeffRef(row, column) = value;
				mini_hessian.coeffRef(column, row) = value;
			}
		}
	}

	SelfAdjointEigenSolver<MatrixXd> es(mini_hessian);
	MatrixXd D = es.eigenvalues().asDiagonal();
	MatrixXd V = es.eigenvectors();
	for (int diag = 0; diag < SeamlessPartials::COUNT; diag++)
	{
		double& diag_entry = D.coeffRef(diag, diag);
		if (diag_entry < 0)
		{
			diag_entry = 0;
		}
	}

	mini_hessian = V * D * V.inverse();

	int ss_index = index;
	for (UpperTriangleEntry& upper_triangle_entry : upper_triangle_entries)
	{
		int mini_hessian_row = hessian_to_mini_hessian_row[upper_triangle_entry.GetRow()];
		int mini_hessian_column = hessian_to_mini_hessian_column[upper_triangle_entry.GetColumn()];
		SS[ss_index++] = mini_hessian.coeffRef(mini_hessian_row, mini_hessian_column);
	}
}

double CrossEnergy::EdgePairElement::GetAtan2Diff()
{
	return atan2_diff_shifted;
}

Eigen::VectorXd CrossEnergy::GetVertexEnergy()
{
	return vertex_energy;
}