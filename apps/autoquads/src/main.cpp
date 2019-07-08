#include <igl/opengl/glfw/Viewer.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <map>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>
#include <plugins/basic-menu/include/basic-menu.h>

#define MODEL1_PATH "..\\..\\..\\models\\cube.off"
#define MODEL2_PATH "..\\..\\..\\models\\cube.off"

int main(int argc, char * argv[])
{
	igl::opengl::glfw::Viewer viewer;

	viewer.load_mesh_from_file(std::string(MODEL1_PATH));
	viewer.load_mesh_from_file(std::string(MODEL2_PATH));

	unsigned int left_view, right_view;
	int model1_id = viewer.data_list[0].id;
	int model2_id = viewer.data_list[1].id;
	viewer.callback_init = [&](igl::opengl::glfw::Viewer &)
	{
		viewer.core().viewport = Eigen::Vector4f(0, 0, 640, 800);
		left_view = viewer.core_list[0].id;
		right_view = viewer.append_core(Eigen::Vector4f(640, 0, 640, 800));

		viewer.data(model1_id).set_visible(false, left_view);
		viewer.data(model2_id).set_visible(false, right_view);

		return false;
	};

	viewer.callback_post_resize = [&](igl::opengl::glfw::Viewer &v, int w, int h) {
		v.core(left_view).viewport = Eigen::Vector4f(0, 0, w / 2, h);
		v.core(right_view).viewport = Eigen::Vector4f(w / 2, 0, w - (w / 2), h);
		return true;
	};

	// Attach a menu plugin
	rds::plugins::BasicMenu menu;
	viewer.plugins.push_back(&menu);

	viewer.launch();
	return EXIT_SUCCESS;
}
