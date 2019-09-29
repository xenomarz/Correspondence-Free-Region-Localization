#pragma once

#ifndef MENU_UTILS_H
#define MENU_UTILS_H

#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/igl_inline.h>
#include <map>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <igl/project.h>
#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_fonts_droid_sans.h>
#include <GLFW/glfw3.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/ViewerPlugin.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <igl/unproject_in_mesh.h>
#include <igl/Hit.h>
#include <igl/rotate_by_quat.h>
#include <memory>
#include <igl/boundary_loop.h>
#include <igl/readOFF.h>
#include <igl/lscm.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>
#include <igl/arap.h>
#include <igl/file_dialog_open.h>
#include <igl/unproject.h>
#include <igl/edge_lengths.h>
#include <igl/slice.h>
#include "../../libs/optimization_lib/include/solvers/NewtonSolver.h"
#include "../../libs/optimization_lib/include/solvers/GradientDescentSolver.h"
#include "../../libs/optimization_lib/include/objective_functions/SymmetricDirichlet.h"
#include "../../libs/optimization_lib/include/objective_functions/SymmetricDirichletCompositeMajorization.h"
#include "../../libs/optimization_lib/include/objective_functions/AreaDistortion.h"
#include "../../libs/optimization_lib/include/objective_functions/AreaDistortionOneRing.h"
#include "../../libs/optimization_lib/include/objective_functions/LeastSquaresConformal.h"
#include "../../libs/optimization_lib/include/objective_functions/PenaltyPositionalConstraints.h"
#include <atomic>

#define RED_COLOR Vector3f(1, 0, 0)
#define BLUE_COLOR Vector3f(0, 0, 1)
#define GREEN_COLOR Vector3f(0, 1, 0)
#define GOLD_COLOR Vector3f(1, 215.0f / 255.0f, 0)
#define GREY_COLOR Vector3f(0.75, 0.75, 0.75)
#define WHITE_COLOR Vector3f(1, 1, 1)
#define BLACK_COLOR Vector3f(0, 0, 0)

using namespace std;
using namespace Eigen;
using namespace igl;
using namespace opengl;
using namespace glfw;
using namespace imgui;

class app_utils : public ImGuiMenu
{
public:
	// Expose an enumeration type
	static enum View {
		Horizontal = 0,
		Vertical = 1,
		InputOnly = 2,
		OutputOnly0 = 3, 
		OutputOnly1 = 4,
		OutputOnly2 = 5,
	};
	static enum outputCores {
		ONE,
		TWO,
		THREE
	};
	static enum MouseMode { 
		NONE = 0, 
		FACE_SELECT, 
		VERTEX_SELECT, 
		CLEAR 
	};
	static enum Parametrization { 
		RANDOM = 0, 
		HARMONIC, 
		LSCM, 
		ARAP, 
		None 
	};
	static enum Distortion { 
		NO_DISTORTION, 
		AREA_DISTORTION, 
		LENGTH_DISTORTION, 
		ANGLE_DISTORTION, 
		TOTAL_DISTORTION 
	};
	static enum SolverType {
		NEWTON,
		GRADIENT_DESCENT
	};

	
	static Vector3f computeTranslation(
		const int mouse_x, 
		const int from_x, 
		const int mouse_y, 
		const int from_y, 
		const RowVector3d pt3D, 
		ViewerCore& core) {
		Matrix4f modelview = core.view;
		//project the given point (typically the handle centroid) to get a screen space depth
		Vector3f proj = project(pt3D.transpose().cast<float>().eval(),
			modelview,
			core.proj,
			core.viewport);
		float depth = proj[2];

		double x, y;
		Vector3f pos1, pos0;

		//unproject from- and to- points
		x = mouse_x;
		y = core.viewport(3) - mouse_y;
		pos1 = unproject(Vector3f(x, y, depth),
			modelview,
			core.proj,
			core.viewport);


		x = from_x;
		y = core.viewport(3) - from_y;
		pos0 = unproject(Vector3f(x, y, depth),
			modelview,
			core.proj,
			core.viewport);

		//translation is the vector connecting the two
		Vector3f translation;
		translation = pos1 - pos0;

		return translation;
	}
	
	static string ExtractModelName(const string& str)
	{
		size_t head, tail;
		head = str.find_last_of("/\\");
		tail = str.find_last_of("/.");
		return (str.substr((head + 1), (tail - head - 1)));
	}

	static bool IsMesh2D(const MatrixXd& V) {
		return (V.col(2).array() == 0).all();
	}

	//Parametrizations
	static void lscm_param(const MatrixXd& V, const MatrixXi& F, MatrixXd& initialguess)
	{
		// Fix two points on the boundary
		VectorXi bnd, b(2, 1);
		boundary_loop(F, bnd);
		b(0) = bnd(0);
		b(1) = bnd(round(bnd.size() / 2));
		MatrixXd bc(2, 2);
		bc << 0, 0, 1, 0;

		lscm(V, F, b, bc, initialguess);
	}

	static void harmonic_param(const MatrixXd& V, const MatrixXi& F, MatrixXd& initialguess) {
		// Find the open boundary
		VectorXi bnd;
		boundary_loop(F, bnd);

		// Map the boundary to a circle, preserving edge proportions
		MatrixXd bnd_uv;
		map_vertices_to_circle(V, bnd, bnd_uv);

		harmonic(V, F, bnd, bnd_uv, 1, initialguess);
	}

	static void ARAP_param(const MatrixXd& V, const MatrixXi& F, MatrixXd& initialguess) {
		// Compute the initial solution for ARAP (harmonic parametrization)
		VectorXi bnd;
		MatrixXd init;

		boundary_loop(F, bnd);
		MatrixXd bnd_uv;
		map_vertices_to_circle(V, bnd, bnd_uv);

		harmonic(V, F, bnd, bnd_uv, 1, init);

		// Add dynamic regularization to avoid to specify boundary conditions
		ARAPData arap_data;
		arap_data.with_dynamics = true;
		VectorXi b = VectorXi::Zero(0);
		MatrixXd bc = MatrixXd::Zero(0, 0);

		// Initialize ARAP
		arap_data.max_iter = 100;
		// 2 means that we're going to *solve* in 2d
		arap_precomputation(V, F, 2, b, arap_data);

		// Solve arap using the harmonic map as initial guess
		initialguess = init;

		arap_solve(bc, arap_data, initialguess);
	}

	static void random_param(const MatrixXd& V, MatrixXd& initialguess) {
		int nvs = V.rows();
		initialguess = MatrixX2d::Random(nvs, 2) * 2.0;
		//MenuUtils::FixFlippedFaces(viewer->data(InputModelID()).F, V_uv);
	}

	static void FixFlippedFaces(MatrixXi& Fs, MatrixXd& Vs) {
		Matrix<double, 3, 2> face_vertices;
		for (MatrixXi::Index i = 0; i < Fs.rows(); ++i)
		{
			slice(Vs, Fs.row(i), 1, face_vertices);
			Vector2d v1_2d = face_vertices.row(1) - face_vertices.row(0);
			Vector2d v2_2d = face_vertices.row(2) - face_vertices.row(0);
			Vector3d v1_3d = Vector3d(v1_2d.x(), v1_2d.y(), 0);
			Vector3d v2_3d = Vector3d(v2_2d.x(), v2_2d.y(), 0);
			Vector3d face_normal = v1_3d.cross(v2_3d);

			// If face is flipped (that is, cross-product do not obey the right-hand rule)
			if (face_normal(2) < 0)
			{
				Fs.row(i) << Fs(i, 0), Fs(i, 2), Fs(i, 1);
			}
		}
	}
	
	static void angle_degree(MatrixXd& V, MatrixXi& F, MatrixXd& angle) {
		int numF = F.rows();
		VectorXd Area;
		MatrixXd Length, alfa, sum;
		ArrayXXd sin_alfa(numF, 3);

		igl::doublearea(V, F, Area);
		igl::edge_lengths(V, F, Length);

		// double_area = a*b*sin(alfa)
		// sin(alfa) = (double_area / a) / b
		sin_alfa.col(0) = Length.col(1).cwiseInverse().cwiseProduct(Length.col(2).cwiseInverse().cwiseProduct(Area));
		sin_alfa.col(1) = Length.col(0).cwiseInverse().cwiseProduct(Length.col(2).cwiseInverse().cwiseProduct(Area));
		sin_alfa.col(2) = Length.col(0).cwiseInverse().cwiseProduct(Length.col(1).cwiseInverse().cwiseProduct(Area));

		// alfa = arcsin ((double_area / a) / b)
		alfa = ((sin_alfa - ArrayXXd::Constant(numF, 3, 1e-10)).asin())*(180 / M_PI);


		//here we deal with errors with sin function
		//especially when the sum of the angles isn't equal to 180!
		sum = alfa.rowwise().sum();
		for (int i = 0; i < alfa.rows(); i++) {
			double diff = 180 - sum(i, 0);
			double c0 = 2 * (90 - alfa(i, 0));
			double c1 = 2 * (90 - alfa(i, 1));
			double c2 = 2 * (90 - alfa(i, 2));

			if ((c0 > (diff - 1)) && (c0 < (diff + 1)))
				alfa(i, 0) += c0;
			else if ((c1 > (diff - 1)) && (c1 < (diff + 1)))
				alfa(i, 1) += c1;
			else if ((c2 > (diff - 1)) && (c2 < (diff + 1)))
				alfa(i, 2) += c2;

			/////////////////////////////////
			//sorting - you can remove this part if the order of angles is important!
			if (alfa(i, 0) > alfa(i, 1)) {
				double temp = alfa(i, 0);
				alfa(i, 0) = alfa(i, 1);
				alfa(i, 1) = temp;
			}
			if (alfa(i, 0) > alfa(i, 2)) {
				double temp = alfa(i, 0);
				alfa(i, 0) = alfa(i, 2);
				alfa(i, 2) = alfa(i, 1);
				alfa(i, 1) = temp;
			}
			else if (alfa(i, 1) > alfa(i, 2)) {
				double temp = alfa(i, 1);
				alfa(i, 1) = alfa(i, 2);
				alfa(i, 2) = temp;
			}
			/////////////////////////////////
		}
		angle = alfa;

		////Checkpoint
		//sum = alfa.rowwise().sum();
		//for (int i = 0; i < alfa.rows(); i++) {
		//	cout << i << ": " << alfa(i, 0) << " " << alfa(i, 1) << " " << alfa(i, 2) << " " << sum.row(i) << endl;
		//}
	}

};

class Output
{
public:
	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional
	MatrixXd color_per_face, Vertices_output;
	int ModelID, CoreID, index;
	ImVec2 window_position, window_size, text_position;

	// Solver thread
	shared_ptr<NewtonSolver> newton;
	shared_ptr<GradientDescentSolver> gradient_descent;
	shared_ptr<solver> solver;
	shared_ptr<TotalObjective> totalObjective;

	//Constructor & initialization
	Output() {
		// Initialize solver thread
		newton = make_shared<NewtonSolver>();
		gradient_descent = make_shared<GradientDescentSolver>();
		solver = newton;
		totalObjective = make_shared<TotalObjective>();
	}
	~Output() {}
};

#endif