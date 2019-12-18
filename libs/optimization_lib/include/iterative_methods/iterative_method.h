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

// LIBIGL includes
#include <igl/flip_avoiding_line_search.h>

// Intel TBB includes
//#include <tbb/concurrent_queue.h>
//#include <tbb/concurrent_vector.h>

// Optimization lib includes
#include "../objective_functions/dense_objective_function.h"
#include "../data_providers/data_provider.h"

// https://en.wikipedia.org/wiki/Iterative_method
template <Eigen::StorageOptions StorageOrder_>
class IterativeMethod
{
public:
	IterativeMethod(std::shared_ptr<ObjectiveFunction<StorageOrder_, Eigen::VectorXd>> objective_function, const Eigen::VectorXd& x0) :
		objective_function_(objective_function),
		x_(x0),
		p_(Eigen::VectorXd::Zero(x0.size())),
		thread_state_(ThreadState::TERMINATED),
		max_backtracking_iterations_(12),
		flip_avoiding_line_search_enabled_(false),
		approximation_invalidated_(false)
	{
		objective_function_->Update(x0, UpdatableObject::UpdatedObjectSet());
	}

	virtual ~IterativeMethod()
	{
		Terminate();
	}

	const std::shared_ptr<ObjectiveFunction<StorageOrder_, Eigen::VectorXd>> GetObjectiveFunction() const
	{
		return objective_function_;
	}

	void Start()
	{
		std::lock_guard<std::mutex> lock(thread_state_mutex_);
		switch (thread_state_)
		{
		case ThreadState::TERMINATED:
			thread_state_ = ThreadState::RUNNING;
			thread_ = std::thread([&]() {
				while (true)
				{
					std::unique_lock<std::mutex> lock(thread_state_mutex_);
					cv_.wait(lock, [&] { return thread_state_ != ThreadState::PAUSED; });

					if (thread_state_ == ThreadState::TERMINATING)
					{
						thread_state_ = ThreadState::TERMINATED;
						break;
					}
					lock.unlock();

					objective_function_->Update(x_, DenseObjectiveFunction<StorageOrder_>::UpdateOptions::GRADIENT | DenseObjectiveFunction<StorageOrder_>::UpdateOptions::HESSIAN);
					ComputeDescentDirection(p_);
					LineSearch(p_);
				}
				});
			break;
		}
	}

	void Pause()
	{
		std::lock_guard<std::mutex> lock(thread_state_mutex_);
		switch (thread_state_)
		{
		case ThreadState::RUNNING:
			thread_state_ = ThreadState::PAUSED;
			break;
		}
	}

	void Resume()
	{
		std::unique_lock<std::mutex> lock(thread_state_mutex_);
		switch (thread_state_)
		{
		case ThreadState::PAUSED:
			thread_state_ = ThreadState::RUNNING;
			cv_.notify_one();
			break;
		case ThreadState::TERMINATED:
			lock.unlock();
			Start();
			break;
		}
	}

	void Terminate()
	{
		std::unique_lock<std::mutex> lock(thread_state_mutex_);
		switch (thread_state_)
		{
		case ThreadState::RUNNING:
			thread_state_ = ThreadState::TERMINATING;
			lock.unlock();
			thread_.join();
			break;
		}
	}

	bool GetApproximation(Eigen::VectorXd& x)
	{
		std::lock_guard<std::mutex> x_lock(x_mutex_);
		if (approximation_invalidated_)
		{
			x = x_;
			approximation_invalidated_ = false;
			return true;
		}
		return false;
	}

	void EnableFlipAvoidingLineSearch(const Eigen::MatrixX3i& F)
	{
		F_ = F;
		flip_avoiding_line_search_enabled_ = true;
	}

	void DisableFlipAvoidingLineSearch()
	{
		flip_avoiding_line_search_enabled_ = false;
	}

private:
	/**
	 * Private data type definitions
	 */
	enum class ThreadState {
		TERMINATED,
		TERMINATING,
		RUNNING,
		PAUSED
	};

	/**
	 * Private methods
	 */
	virtual void ComputeDescentDirection(Eigen::VectorXd& p) = 0;
	
	void LineSearch(const Eigen::VectorXd& p)
	{
		/**
		 * Calculate maximal flip avoiding step-size
		 * https://github.com/libigl/libigl/blob/master/include/igl/flip_avoiding_line_search.cpp
		 */
		double step_size;
		if (flip_avoiding_line_search_enabled_)
		{
			Eigen::MatrixX2d mat_x = Eigen::Map<Eigen::MatrixX2d>(x_.data(), x_.rows() / 2, 2);
			Eigen::MatrixXd mat_p = Eigen::Map<const Eigen::MatrixX2d>(p.data(), p.rows() / 2, 2);
			double min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(mat_x, F_, mat_p);
			step_size = std::min(1., min_step_to_singularity * 0.8);
		}
		else
		{
			step_size = 1;
		}

		/**
		 * Perform backtracking (armijo rule)
		 * https://en.wikipedia.org/wiki/Backtracking_line_search
		 */
		double current_value = objective_function_->GetValue();
		double updated_value;
		int current_iteration = 0;
		Eigen::MatrixXd current_x;
		while (current_iteration < max_backtracking_iterations_)
		{
			current_x = x_ + step_size * p;
			objective_function_->Update(current_x, DenseObjectiveFunction<StorageOrder_>::UpdateOptions::VALUE);
			updated_value = objective_function_->GetValue();

			if (updated_value >= current_value)
			{
				step_size /= 2;
			}
			else
			{
				break;
			}

			current_iteration++;
		}

		objective_function_->Update(current_x, DenseObjectiveFunction<StorageOrder_>::UpdateOptions::VALUE_PER_VERTEX);

		std::lock_guard<std::mutex> x_lock(x_mutex_);
		x_ = std::move(current_x);
		approximation_invalidated_ = true;
	}

	/**
	 * Fields
	 */

	// Synchronization objects
	std::thread thread_;
	std::condition_variable cv_;
	mutable std::mutex thread_state_mutex_;
	mutable std::mutex x_mutex_;

	// Objective function
	std::shared_ptr<ObjectiveFunction<StorageOrder_, Eigen::VectorXd>> objective_function_;

	// Line search
	const long max_backtracking_iterations_;

	// Flags and states
	ThreadState thread_state_;
	bool flip_avoiding_line_search_enabled_;
	bool approximation_invalidated_;

	// Current approximation and descent direction
	Eigen::VectorXd x_;
	Eigen::VectorXd p_;

	// Faces
	Eigen::MatrixX3i F_;
};

#endif