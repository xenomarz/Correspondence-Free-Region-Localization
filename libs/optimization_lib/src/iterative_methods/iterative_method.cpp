// Optimization lib includes
#include <iterative_methods/iterative_method.h>

// LIBIGL includes
#include <igl/flip_avoiding_line_search.h>

IterativeMethod::IterativeMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
	objective_function_(objective_function),
	x_(x0),
	p_(Eigen::VectorXd::Zero(x0.size())),
	thread_state_(ThreadState::TERMINATED),
	max_backtracking_iterations_(2),
	flip_avoiding_line_search_enabled_(false),
	approximation_invalidated_(false)
{
	objective_function_->Update(x0);
}

IterativeMethod::~IterativeMethod()
{
	Terminate();
}

const std::shared_ptr<ObjectiveFunction> IterativeMethod::GetObjectiveFunction() const
{
	return objective_function_;
}

void IterativeMethod::LineSearch(const Eigen::VectorXd& p)
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
		objective_function_->Update(current_x, ObjectiveFunction::UpdateOptions::VALUE);
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

	std::lock_guard<std::mutex> x_lock(x_mutex_);
	x_ = std::move(current_x);
	approximation_invalidated_ = true;
}

void IterativeMethod::Start()
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

				objective_function_->Update(x_, ObjectiveFunction::UpdateOptions::GRADIENT | ObjectiveFunction::UpdateOptions::HESSIAN);
				ComputeDescentDirection(p_);
				LineSearch(p_);
			}
		});
		break;
	}
}

void IterativeMethod::Pause()
{
	std::lock_guard<std::mutex> lock(thread_state_mutex_);
	switch (thread_state_)
	{
	case ThreadState::RUNNING:
		thread_state_ = ThreadState::PAUSED;
		break;
	}
}

void IterativeMethod::Resume()
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

void IterativeMethod::Terminate()
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

bool IterativeMethod::GetApproximation(Eigen::VectorXd& x)
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

void IterativeMethod::EnableFlipAvoidingLineSearch(const Eigen::MatrixX3i& F)
{
	F_ = F;
	flip_avoiding_line_search_enabled_ = true;
}

void IterativeMethod::DisableFlipAvoidingLineSearch()
{
	flip_avoiding_line_search_enabled_ = false;
}