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




using namespace std;

#define MODEL1_PATH "..\\..\\..\\models\\wolf.obj"
#define MODEL2_PATH "..\\..\\..\\models\\cow.obj"

int main(int argc, char * argv[])
{
	static int counter = 0;
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

		viewer.core(0).align_camera_center(viewer.data(0).V, viewer.data(0).F);
		viewer.core(1).align_camera_center(viewer.data(1).V, viewer.data(1).F);

		return false;
	};

	viewer.callback_post_resize = [&](igl::opengl::glfw::Viewer &v, int w, int h) {
		if (view == Two_views) {
			v.core(left_view).viewport = Eigen::Vector4f(0, 0, w / 2, h);
			v.core(right_view).viewport = Eigen::Vector4f(w / 2, 0, w - (w / 2), h);
		}
		if (view == Left_view) {
			v.core_list[left_view].viewport = Eigen::Vector4f(0, 0, w, h);
			v.core_list[right_view].viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
		}
		if (view == Right_view) {
			v.core_list[left_view].viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
			v.core_list[right_view].viewport = Eigen::Vector4f(0, 0, w, h);
		}	    

		
		return true;
	};
	
	// Attach a menu plugin
	rds::plugins::BasicMenu menu;
	
	viewer.plugins.push_back(&menu);
	viewer.launch();
	return EXIT_SUCCESS;
}
