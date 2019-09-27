#include <basic_app/include/core.h>

Output::Output() {
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


	distortion_type = app_utils::TOTAL_DISTORTION;
	solver_type = app_utils::GRADIENT_DESCENT;

	// Initialize solver thread
	newton = make_shared<NewtonSolver>();
	gradient_descent = make_shared<GradientDescentSolver>();
	solver = newton;
	totalObjective = make_shared<TotalObjective>();
}

void Output::stop_solver_thread() {
	solver_on = false;
	if (solver->is_running) {
		solver->stop();
	}
	while (solver->is_running);
}

void Output::start_solver_thread(thread& t) {
	if (!solverInitialized) {
		solver_on = false;
		return;
	}
	cout << ">> start new solver" << endl;
	solver_on = true;

	t = thread(&solver::run, solver.get());
	t.detach();
}