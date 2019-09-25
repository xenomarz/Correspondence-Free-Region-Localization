#pragma once

#include "EigenTypes.h"
#include "ObjectiveFunction.h"
#include "MeshWrapper.h"
#include "Energies/Seamless/Atan2Diff.h"
#include "Energies/Seamless/CoordDiffRatio.h"
#include "UpperTriangleEntry.h"

#include <list>
#include <vector>
#include <map>
#include <utility>

using namespace std;

#ifndef INF
#define INF numeric_limits<double>::infinity()
#endif

class CrossEnergy : public ObjectiveFunction
{
	typedef std::map<int, std::vector<int>> EdgeToCopies;
	typedef std::pair<int, int> EdgePair;
	typedef std::vector<EdgePair> EdgePairs;
	typedef std::pair<int, int> EdgeVertices;
	typedef std::pair<EdgeVertices, EdgeVertices> EdgePairVertices;

	class EdgePairElement {
	public:
		EdgePairElement(shared_ptr<MeshWrapper> mesh_wrapper, EdgePair edge_pair, EdgePairVertices edge_pair_vertices);
		int GetEdge1Vertex1();
		int GetEdge1Vertex2();
		int GetEdge2Vertex1();
		int GetEdge2Vertex2();
		int GetEdge1();
		int GetEdge2();
		const Vector2d& GetEdge1Vector();
		const Vector2d& GetEdge2Vector();
		void UpdateEdgeVectors(const Eigen::MatrixX2d& X);
		double Value();
		void Gradient(MatX2& g_mat);
		void Hessian(std::vector<double>& SS, int index);
		double GetAtan2Diff();
		std::vector<UpperTriangleEntry>& GetUpperTriangleEntries();
		void CalculateRelativeOriginalLength(double total_length);
		double GetOriginalLength();

		std::map<int, int> mini_hessian_to_hessian_row;
		std::map<int, int> hessian_to_mini_hessian_row;
		std::map<int, int> mini_hessian_to_hessian_column;
		std::map<int, int> hessian_to_mini_hessian_column;
		std::map<std::pair<int, int>, UpperTriangleEntry> mini_hessian_row_coumn_to_upper_triangle_entry;

	private:
		bool first_update;
		double original_length;
		double relative_original_length;
		
		static double c0;
		static double c1;
		static double c2;
		static double c3;

		static double c2_mult_2;
		static double c3_mult_3;
		static double c3_mult_6;

		static double pi_1_4;
		static double pi_1_2;
		static double pi_3_4;

		inline double FirstPartialDerivative(int partial);
		inline double SecondPartialDerivative(int first_partial, int second_partial);
		inline void BuildUpperTriangleEntries();

		EdgePair edge_pair;
		EdgePairVertices edge_pair_vertices;

		double atan2_diff;
		double atan2_diff_mod;
		double atan2_diff_shifted;
		double atan2_diff_shifted_squared;
		double atan2_diff_shifted_tripled;
		double sign;
		double sign2;

		shared_ptr<MeshWrapper> mesh_wrapper;

		std::vector<UpperTriangleEntry> upper_triangle_entries;

		Atan2Diff atan2_diff_exp;
	};

	typedef std::vector<shared_ptr<EdgePairElement>> EdgePairElements;

public:
	CrossEnergy();
	~CrossEnergy();

	void init();
	void updateX(const VectorXd &X);
	double value();
	void gradient(VectorXd &g);
	void hessian();
	void prepare_hessian();

	std::vector<int> GetSeamlessHistogram(int buckets);

	shared_ptr<MeshWrapper> mesh_wrapper;
	EdgePairElements edge_pair_elements;

	Eigen::VectorXd GetVertexEnergy();

private:
	bool first_update;
	Eigen::VectorXd vertex_energy;

	inline static std::vector<std::pair<int, int>> GetAllPairCombinations(const std::vector<int>& indices);
};