#pragma once

#include <libs/optimization_lib/include/objective_functions/TotalObjective.h>
#include <libs/optimization_lib/include/objective_functions/ConstrainedObjectiveFunction.h>
#include <atomic>
#include <shared_mutex>
#include <igl/flip_avoiding_line_search.h>
#include <Eigen/SparseCholesky>
#include <igl/matlab_format.h>
#include <igl/matlab/MatlabWorkspace.h>
#include <igl/matlab/matlabinterface.h>

class solver
{
public:
	solver(const bool IsConstrObjFunc, const int solverID);
	~solver();
	int run();
	void run_one_iteration(const int steps);
	void stop();
	void get_data(Eigen::VectorXd& X);
	void init(std::shared_ptr<ObjectiveFunction> objective, const Eigen::VectorXd& X0);
	void setFlipAvoidingLineSearch(Eigen::MatrixX3i& F);

	// Pointer to the energy class
	std::shared_ptr<ObjectiveFunction> objective;

	// Activity flags
	std::atomic_bool is_running = {false};
	std::atomic_bool progressed = {false};

	// Synchronization functions used by the wrapper
	void wait_for_parameter_update_slot();
	void release_parameter_update_slot();

	// External (interface) and internal working mesh
	Eigen::VectorXd ext_x;
	Eigen::VectorXd X;
	Eigen::MatrixX3i F;
	int num_steps;
	bool IsConstrObjFunc;
	Utils::LineSearch lineSearch_type;
protected:
	// Give the wrapper a chance to intersect gracefully
	void give_parameter_update_slot();
	// Updating the data after a step has been done
	void update_external_data();

	// Descent direction evaluated in step
	Eigen::VectorXd p;
	
	// Current energy, gradient and hessian
	double f;
	Eigen::VectorXd g;

	// Synchronization structures
	std::atomic_bool params_ready_to_update = {false};
	std::atomic_bool wait_for_param_update = {false};
	std::atomic_bool a_parameter_was_updated = {false};
	std::atomic_bool halt = {false};
	
	std::unique_ptr<std::shared_timed_mutex> data_mutex;

	// pardiso variables
	//vector<int> II, JJ;
	//vector<double> SS;

private:
	// Matlab instance
	Engine *engine;

	int solverID;
	// energy output from the last step
	double currentEnergy;
	virtual double step() = 0;
	virtual void value_linesearch(std::ofstream& SearchDirInfo);
	virtual void gradNorm_linesearch(std::ofstream& SearchDirInfo);
	virtual bool test_progress() = 0;
	virtual void internal_init() = 0;
	void prepareData();
	void saveSearchDirInfo(int numIteration, std::ofstream& SearchDirInfo);
	void saveSolverInfo(int numIteration, std::ofstream& solverInfo);
	void saveHessianInfo(int numIteration, std::ofstream& hessianInfo);
	void sendDataToMatlab();
	//CSV output
	Eigen::SparseMatrix<double> CurrHessian;
	Eigen::MatrixXd 
		lineSearch_alfa,
		lineSearch_value, 
		lineSearch_augmentedValue, 
		lineSearch_gradientNorm;
	double step_size;
	int cur_iter;
	Eigen::VectorXd X_before;
	std::ofstream SearchDirInfo, solverInfo, hessianInfo;

	// Mutex stuff
	std::unique_ptr<std::mutex> parameters_mutex;
	std::unique_ptr<std::condition_variable> param_cv;
	bool FlipAvoidingLineSearch = false;
};