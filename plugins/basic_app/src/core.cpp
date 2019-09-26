#include <basic_app/include/core.h>

Core::Core(int index) {
	this->index = index;
	core_size = 1.0 / 3.0;
}

void Core::setName(string mesh_name) {
	name = mesh_name + " (Param. " + std::to_string(index) + ")";
}