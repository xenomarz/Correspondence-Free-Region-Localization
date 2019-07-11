#include <igl/opengl/glfw/Viewer.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <map>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>
#include <plugins/basic-menu/include/basic-menu.h>
#include <iostream>

//
//void compute_harmonicsc_param(igl::opengl::glfw::Viewer viewer, int model_index) {
//	// Find the open boundary
//	Eigen::VectorXi bnd;
//	Eigen::MatrixXd V_uv;
//	igl::boundary_loop(viewer.data(model_index).F, bnd);
//
//	// Map the boundary to a circle, preserving edge proportions
//	Eigen::MatrixXd bnd_uv;
//	igl::map_vertices_to_circle(viewer.data(model_index).V, bnd, bnd_uv);
//
//	// Harmonic parametrization for the internal vertices
//	igl::harmonic(viewer.data(model_index).V, viewer.data(model_index).F, bnd, bnd_uv, 1, V_uv);
//
//	// Scale UV to make the texture more clear
//	V_uv *= 5;
//
//	// Plot the mesh
//	viewer.data(model_index).set_mesh(viewer.data(model_index).V, viewer.data(model_index).F);
//	viewer.data(model_index).set_uv(V_uv);
//
//	viewer.data(model_index).set_mesh(viewer.data(model_index).V_uv, viewer.data(model_index).F);
//
//	viewer.data(model_index).compute_normals();
//
//	// Draw checkerboard texture
//	viewer.data(model_index).show_texture = true;
//}

using namespace std;
using namespace Eigen;

#define MODEL2_PATH "..\\..\\..\\models\\camel_head.off"
#define MODEL1_PATH "..\\..\\..\\models\\cube.off"


int main(int argc, char * argv[])
{
	static int counter = 0;
	igl::opengl::glfw::Viewer viewer;

	viewer.load_mesh_from_file(std::string(MODEL1_PATH));
	viewer.load_mesh_from_file(std::string(MODEL1_PATH));
	//viewer.load_mesh_from_file(std::string(MODEL2_PATH));

	
	unsigned int left_view, right_view;
	int model0_id = viewer.data_list[0].id;
	int model1_id = viewer.data_list[1].id;

	


	viewer.callback_init = [&](igl::opengl::glfw::Viewer &)
	{
		viewer.core().viewport = Eigen::Vector4f(0, 0, 640, 800);
		left_view = viewer.core(0).id;
		right_view = viewer.append_core(Eigen::Vector4f(640, 0, 640, 800));
		//compute_harmonicsc_param(viewer, 1);
		viewer.data(model1_id).set_visible(false, left_view);
		viewer.data(model0_id).set_visible(false, right_view);

		viewer.core(left_view).align_camera_center(viewer.data(model0_id).V, viewer.data(model0_id).F);
		viewer.core(right_view).align_camera_center(viewer.data(model1_id).V, viewer.data(model1_id).F);
		return false;
	};

	viewer.callback_post_resize = [&](igl::opengl::glfw::Viewer &v, int w, int h) {
		if (view == Two_views) {
			v.core(left_view).viewport = Eigen::Vector4f(0, 0, w / 2, h);
			v.core(right_view).viewport = Eigen::Vector4f(w / 2, 0, w - (w / 2), h);
		}
		if (view == Left_view) {
			v.core(left_view).viewport = Eigen::Vector4f(0, 0, w, h);
			v.core(right_view).viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
		}
		if (view == Right_view) {
			v.core(left_view).viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
			v.core(right_view).viewport = Eigen::Vector4f(0, 0, w, h);
		}	    
		return true;
	};
	
	// Attach a menu plugin
	rds::plugins::BasicMenu menu;
	
	viewer.plugins.push_back(&menu);
	
	viewer.launch();
	return EXIT_SUCCESS;
}
