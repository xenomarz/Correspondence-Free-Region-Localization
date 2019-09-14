#include <plugins/basic-menu/include/basic_menu.h>

int main(int argc, char * argv[])
{
	igl::opengl::glfw::Viewer viewer;
	BasicMenu menu;
	viewer.plugins.push_back(&menu);
	viewer.launch();
	return EXIT_SUCCESS;
}
