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


int main(int argc, char * argv[])
{
	igl::opengl::glfw::Viewer viewer;

	viewer.callback_init = [&](igl::opengl::glfw::Viewer &)
	{
		viewer.load_mesh_from_file(std::string(MODEL1_PATH));
		viewer.load_mesh_from_file(std::string(MODEL1_PATH));

		unsigned int left_view, right_view;
		int model0_id = viewer.data_list[0].id;
		int model1_id = viewer.data_list[1].id;

		viewer.core().viewport = Eigen::Vector4f(0, 0, 640, 800);
		left_view = viewer.core(0).id;
		right_view = viewer.append_core(Eigen::Vector4f(640, 0, 640, 800));
		viewer.data(model1_id).set_visible(false, left_view);
		viewer.data(model0_id).set_visible(false, right_view);

		viewer.core(left_view).align_camera_center(viewer.data(model0_id).V, viewer.data(model0_id).F);
		viewer.core(right_view).align_camera_center(viewer.data(model1_id).V, viewer.data(model1_id).F);
		return false;
	};
	
	// Attach a menu plugin
	rds::plugins::BasicMenu menu;
	
	viewer.plugins.push_back(&menu);
	
	viewer.launch();
	return EXIT_SUCCESS;
}
