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
	
	// Attach a menu plugin
	rds::plugins::BasicMenu menu;
	
	viewer.plugins.push_back(&menu);
	
	viewer.launch();
	return EXIT_SUCCESS;
}
