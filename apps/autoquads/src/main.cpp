#include <plugins/basic-menu/include/basic-menu.h>
#include <libs/optimization_lib/include/solvers/solver.h>

int main(int argc, char * argv[])
{
	igl::opengl::glfw::Viewer viewer;
	rds::plugins::BasicMenu menu;
	viewer.plugins.push_back(&menu);
	viewer.launch();
	return EXIT_SUCCESS;
}
