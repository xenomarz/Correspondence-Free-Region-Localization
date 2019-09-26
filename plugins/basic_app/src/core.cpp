#include <basic_app/include/core.h>

Core::Core(int index) {
	this->index = index;
	core_size = 1.0 / 3.0;
	Max_Distortion = 5;

	Highlighted_face_color = RED_COLOR;
	Fixed_face_color = BLUE_COLOR;
	Dragged_face_color = GREEN_COLOR;
	Vertex_Energy_color = RED_COLOR;
	Dragged_vertex_color = GREEN_COLOR;
	Fixed_vertex_color = BLUE_COLOR;
	model_color = GREY_COLOR;
	text_color = BLACK_COLOR;

	param_type = app_utils::None;
	Highlighted_face = false;
	texture_scaling_output = 1;
}

void Core::setName(string mesh_name) {
	name = mesh_name + " (Param. " + std::to_string(index) + ")";
}