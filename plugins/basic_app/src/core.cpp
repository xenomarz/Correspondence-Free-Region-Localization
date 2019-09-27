#include <basic_app/include/core.h>

Output::Output() {
	// Initialize solver thread
	newton = make_shared<NewtonSolver>();
	gradient_descent = make_shared<GradientDescentSolver>();
	solver = newton;
	totalObjective = make_shared<TotalObjective>();
}

void Output::stop_solver_thread(bool& solver_on) {
	solver_on = false;
	if (solver->is_running) {
		solver->stop();
	}
	while (solver->is_running);
}

void Output::start_solver_thread(thread& t, bool& solver_on) {
	if (!solverInitialized) {
		solver_on = false;
		return;
	}
	cout << ">> start new solver" << endl;
	solver_on = true;

	t = thread(&solver::run, solver.get());
	t.detach();
}