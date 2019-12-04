#pragma once

#include <libs/optimization_lib/include/objective_functions/TotalObjective.h>
#include <atomic>
#include <shared_mutex>
#include <igl/flip_avoiding_line_search.h>
#include <Eigen/SparseCholesky>

class solver
{
public:
	solver(const bool IsConstrObjFunc, const int solverID);
	~solver();
	int run();
	void stop();
	void get_data(VectorXd& X);
	void init(shared_ptr<ObjectiveFunction> objective, const VectorXd& X0);
	void setFlipAvoidingLineSearch(MatrixX3i& F);

	// Pointer to the energy class
	shared_ptr<ObjectiveFunction> objective;

	// Activity flags
	atomic_bool is_running = {false};
	atomic_bool progressed = {false};

	// Synchronization functions used by the wrapper
	void wait_for_parameter_update_slot();
	void release_parameter_update_slot();

	// External (interface) and internal working mesh
	VectorXd ext_x, X;
	MatrixX3i F;
	int num_steps;
	bool IsConstrObjFunc;
protected:
	// Give the wrapper a chance to intersect gracefully
	void give_parameter_update_slot();
	// Updating the data after a step has been done
	void update_external_data();

	// Descent direction evaluated in step
	VectorXd p;
	
	// Current energy, gradient and hessian
	double f;
	VectorXd g;

	// Synchronization structures
	atomic_bool params_ready_to_update = {false};
	atomic_bool wait_for_param_update = {false};
	atomic_bool a_parameter_was_updated = {false};
	atomic_bool halt = {false};
	
	unique_ptr<shared_timed_mutex> data_mutex;

	// pardiso variables
	//vector<int> II, JJ;
	//vector<double> SS;

private:
	int solverID;
	// energy output from the last step
	double currentEnergy;
	virtual double step() = 0;
	virtual void linesearch(std::ofstream& SearchDirInfo);
	virtual bool test_progress() = 0;
	virtual void internal_init() = 0;
	void saveSearchDirInfo(int numIteration, std::ofstream& SearchDirInfo);
	void saveSolverInfo(int numIteration, std::ofstream& solverInfo);
	void saveHessianInfo(int numIteration, std::ofstream& hessianInfo);
	double alfa[601] = { 0 };
	double y_value[601] = { 0 };
	double y_augmentedValue[601] = { 0 };
	std::ofstream SearchDirInfo, solverInfo, hessianInfo;

	// Mutex stuff
	unique_ptr<mutex> parameters_mutex;
	unique_ptr<condition_variable> param_cv;
	bool FlipAvoidingLineSearch = false;
};