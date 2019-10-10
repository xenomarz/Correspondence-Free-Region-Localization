#include "IntegerEnergy.h"
#include "autodiff.h"

#include <omp.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <igl/cat.h>
#include <limits>

#define _USE_MATH_DEFINES
#include <cmath>  

using namespace std;

IntegerEnergy::IntegerEnergy()
{
}

IntegerEnergy::~IntegerEnergy()
{
}

void IntegerEnergy::init()
{
    igl::cat(1, EVvar1, EVvar2, Eint_orig);
    Eint = Eint_orig;
    init_selected_vertices();
    init_soup_to_model_map();
    update_energy_matrix();
    Eintt = Eint.transpose();
    updateK(1);


	dMat = MatrixXd(Eint_orig);
}

void IntegerEnergy::updateK(double k)
{
    if (k < std::numeric_limits<double>::epsilon())
    {
        k = std::numeric_limits<double>::epsilon();
    }

    this->k = k;
    c = (2 * M_PI) / k;
}

double IntegerEnergy::value()
{
    return Eint_X.unaryExpr([&](double x) { return cos((c * x) - M_PI) + 1; }).rowwise().sum().sum();
}

void IntegerEnergy::gradient(VectorXd& g)
{
    MatX2 g_MatX2 = Eint.transpose() * (-c*Eint_X.unaryExpr([&](double x) { return sin((c * x) - M_PI); }));
    g = Eigen::Map<Vec>(g_MatX2.data(), 2.0 * g_MatX2.rows(), 1);
}

void IntegerEnergy::hessian()
{
    int n = numV;
    int threads = omp_get_max_threads();

    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < Eintt.outerSize(); ++i)
    {
        // no inner loop because there are only 2 nnz values per col
        int tid = omp_get_thread_num();

        SpMat::InnerIterator it(Eintt, i);

        int idx_xi = it.row();
        double factor_xi = double(it.value())*c;

        ++it;
        int idx_xj = it.row();
        double factor_xj = double(it.value())*c;
        
        Vec2 xi = X.row(idx_xi);
        Vec2 xj = X.row(idx_xj);

        double ddf_x = -cos(factor_xi * xi[0] + factor_xj * xj[0] - M_PI);
        double ddf_y = -cos(factor_xi * xi[1] + factor_xj * xj[1] - M_PI);

        double ddxi_x_ddxi_x = (factor_xi * factor_xi) * ddf_x;
        double ddxi_y_ddxi_y = (factor_xi * factor_xi) * ddf_y;

        double ddxi_x_ddxj_x = (factor_xi * factor_xj) * ddf_x;
        double ddxi_y_ddxj_y = (factor_xi * factor_xj) * ddf_y;

        double ddxj_x_ddxj_x = (factor_xj * factor_xj) * ddf_x;
        double ddxj_y_ddxj_y = (factor_xj * factor_xj) * ddf_y;

        int ss_ind = 6 * i;

        SS[ss_ind++] = ddxi_x_ddxi_x;
        SS[ss_ind++] = ddxi_y_ddxi_y;

        SS[ss_ind++] = ddxi_x_ddxj_x;
        SS[ss_ind++] = ddxi_y_ddxj_y;

        SS[ss_ind++] = ddxj_x_ddxj_x;
        SS[ss_ind++] = ddxj_y_ddxj_y;
    }
}

void IntegerEnergy::updateX(const VectorXd& X)
{
    this->X.resize(numV, Eigen::NoChange);
    for (int i = 0; i < numV; i++)
    {
        this->X.row(i) << X(i), X(i + numV);
    }

    Eint_X = Eint * this->X;
}

void IntegerEnergy::prepare_hessian()
{
    int n = numV;
    II.clear();
    JJ.clear();
    auto PushPair = [&](int i, int j) { II.push_back(i); JJ.push_back(j); };
    for (int i = 0; i < Eintt.outerSize(); ++i)
    {
        SpMat::InnerIterator it(Eintt, i);
        int idx_xi = it.row();
        int idx_xj = (++it).row();

        PushPair(idx_xi, idx_xi);
        PushPair(idx_xi + n, idx_xi + n);

        PushPair(idx_xi, idx_xj);
        PushPair(idx_xi + n, idx_xj + n);

        PushPair(idx_xj, idx_xj);
        PushPair(idx_xj + n, idx_xj + n);
    }

    SS = vector<double>(II.size(), 0.);
}

void IntegerEnergy::add_vertex(int vid)
{
    std::vector<int>::iterator it = std::find(selectedVertices.begin(), selectedVertices.end(), vid);
    if (it == selectedVertices.end())
    {
        selectedVertices.push_back(vid);
    }

    update_energy_matrix();
}

void IntegerEnergy::remove_vertex(int vid)
{
    std::vector<int>::iterator it = std::find(selectedVertices.begin(), selectedVertices.end(), vid);
    if (it != selectedVertices.end())
    {
        selectedVertices.erase(it);
    }

    update_energy_matrix();
}

void IntegerEnergy::update_energy_matrix()
{
    for (int i = 0; i < Eint_orig.outerSize(); ++i) 
	{
        for (SpMat::InnerIterator it(Eint_orig, i); it; ++it)
        {
            int modelVertexId = soupVertexIdToModelVetexId[it.col()];
            std::vector<int>::iterator selectedVertices_it = std::find(selectedVertices.begin(), selectedVertices.end(), modelVertexId);
            if (selectedVertices_it != selectedVertices.end())
            {
                Eint.coeffRef(it.row(), it.col()) = 0;
            }
            else
            {
                Eint.coeffRef(it.row(), it.col()) = Eint_orig.coeffRef(it.row(), it.col());
            }
        }
    }

	Eintt = Eint.transpose();
	prepare_hessian();
}

std::vector<int>& IntegerEnergy::get_selected_vertices()
{
    return this->selectedVertices;
}

void IntegerEnergy::init_selected_vertices()
{
    for (int i = 0; i < V2V.rows(); i++)
    {
        selectedVertices.push_back(i);
    }
}

void IntegerEnergy::init_soup_to_model_map()
{
	for (int i = 0; i < V2V.outerSize(); ++i) 
	{
		for (SpMati::InnerIterator it(V2V, i); it; ++it)
		{
			soupVertexIdToModelVetexId[it.col()] = it.row();
		}
	}

    //for (int i = 0; i < V2V.rows(); i++)
    //{
    //    for (int j = 0; j < V2V.cols(); j++)
    //    {
    //        if (V2V.coeffRef(i, j))
    //        {
    //            soupVertexIdToModelVetexId[j] = i;
    //        }
    //    }
    //}
}