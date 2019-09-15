#pragma once
#ifndef OPTIMIZATION_LIB_ITERATIVE_METHOD
#define OPTIMIZATION_LIB_ITERATIVE_METHOD

// STL includes
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

// Eigen includes
#include <Eigen/Core>
#include <Eigen/Sparse>

// Intel TBB includes
#include <tbb/concurrent_queue.h>

// Optimization lib includes
#include <objective_functions/objective_function.h>

// https://en.wikipedia.org/wiki/Iterative_method
class IterativeMethod
{
public:
	IterativeMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0);
	virtual ~IterativeMethod();

	virtual void Run();
	virtual void Stop();
	void GetApproximation(Eigen::VectorXd& x);
	void EnableFlipAvoidingLineSearch(Eigen::MatrixX3i& f);
	void DisableFlipAvoidingLineSearch(Eigen::MatrixX3i& f);

private:
	// Iteration step
	virtual void Step(Eigen::VectorXd& p) = 0;
	std::thread thread_;

	// Objective function
	std::shared_ptr<ObjectiveFunction> objective_function_;

	// Flags
	std::atomic_bool is_running_;
	std::atomic_bool progressed_;
	bool flip_avoiding_line_search_enabled_;

	// Concurrent queue of solution approximations
	tbb::concurrent_queue<Eigen::VectorXd> approximations_queue_;

	// Current approximation and descent direction
	Eigen::VectorXd x_;
	Eigen::VectorXd p_;

	// Faces
	Eigen::MatrixX3i f;
};

#endif