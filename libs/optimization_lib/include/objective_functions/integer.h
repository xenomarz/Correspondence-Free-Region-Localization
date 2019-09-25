#pragma once

#include "EigenTypes.h"
#include "ObjectiveFunction.h"

#include <list>
#include <vector>

using namespace std;

#ifndef INF
#define INF numeric_limits<double>::infinity()
#endif

class IntegerEnergy : public ObjectiveFunction
{
  public:
    IntegerEnergy();
    ~IntegerEnergy();

    void init();
    void updateX(const VectorXd& X);
    double value();
    void gradient(VectorXd& g);
    void hessian();
    void prepare_hessian();
    void updateK(double k);
    std::vector<int>& get_selected_vertices();
    void add_vertex(int vid);
    void remove_vertex(int vid);
    void update_energy_matrix();
    void init_selected_vertices();
    void init_soup_to_model_map();

    Eigen::MatrixX2d X;
    SpMat EVvar1, EVvar2, Eint, Eint_orig, Eintt;
	SpMati V2V;
    MatX2 Eint_X;
    Vec EintX_cosine_rowwise_sum;
    int numV;
    double k;
    double c;
	MatrixXd dMat;

    std::vector<int> selectedVertices;
    std::map<int, int> soupVertexIdToModelVetexId;
};