#pragma once
#ifndef OPTIMIZATION_LIB_ITERATIVE_METHOD_H
#define OPTIMIZATION_LIB_ITERATIVE_METHOD_H

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
#include "../objective_functions/objective_function.h"

// https://en.wikipedia.org/wiki/Iterative_method
class IterativeMethod
{
public:
	IterativeMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0);
	virtual ~IterativeMethod();

	void Start();
	void Pause();
	void Resume();
	void Shutdown();
	bool GetApproximation(Eigen::VectorXd& x);
	void EnableFlipAvoidingLineSearch(Eigen::MatrixX3i& f);
	void DisableFlipAvoidingLineSearch();
	const std::shared_ptr<ObjectiveFunction> GetObjectiveFunction() const;

private:
	enum class ThreadState {
		SHUTDOWN,
		RUNNING,
		PAUSED
	};

	// Iteration step
	virtual void ComputeDescentDirection(Eigen::VectorXd& p) = 0;
	void LineSearch(const Eigen::VectorXd& p);

	std::thread thread_;
	std::condition_variable cv_;
	std::mutex m_;
	const double max_backtracking_iterations_;

	// Objective function
	std::shared_ptr<ObjectiveFunction> objective_function_;

	// Flags
	ThreadState thread_state_;
	bool flip_avoiding_line_search_enabled_;

	// Concurrent queue of solution approximations
	tbb::concurrent_queue<Eigen::VectorXd> approximations_queue_;

	// Current approximation and descent direction
	Eigen::VectorXd x_;
	Eigen::VectorXd p_;

	// Faces
	Eigen::MatrixX3i F_;
};

#endif