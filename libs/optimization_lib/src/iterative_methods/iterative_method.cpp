// Optimization lib includes
#include <iterative_methods/iterative_method.h>

// LIBIGL includes
#include <igl/flip_avoiding_line_search.h>

IterativeMethod::IterativeMethod(std::shared_ptr<ObjectiveFunction> objective_function, const Eigen::VectorXd& x0) :
	objective_function_(objective_function),
	x_(x0),
	thread_state_(ThreadState::SHUTDOWN),
	max_backtracking_iterations_(10)
{

}

IterativeMethod::~IterativeMethod()
{
	Shutdown();
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
		double min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(mat_x, f_, mat_p);
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
		objective_function_->Update(current_x);
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

	x_ = std::move(current_x);
}

void IterativeMethod::Start()
{
	if (thread_.joinable())
	{
		Shutdown();
	}

	thread_ = std::thread([&]() {
		while (true)
		{
			std::unique_lock<decltype(m_)> lock(m_);
			cv_.wait(lock, [this] { return thread_state_ != ThreadState::PAUSED; });

			if (thread_state_ == ThreadState::SHUTDOWN)
			{
				break;
			}

			ComputeDescentDirection(p_);
			LineSearch(p_);
			approximations_queue_.push(x_);
		}
	});
}

void IterativeMethod::Pause()
{
	::std::unique_lock<decltype(m_)> lock(m_);
	thread_state_ = ThreadState::PAUSED;
}

void IterativeMethod::Resume()
{
	{
		::std::unique_lock<decltype(m_)> lock(m_);
		thread_state_ = ThreadState::RUNNING;
	}
	cv_.notify_one();
}

void IterativeMethod::Shutdown()
{
	{
		::std::unique_lock<decltype(m_)> lock(m_);
		thread_state_ = ThreadState::SHUTDOWN;
	}
	thread_.join();
}

bool IterativeMethod::GetApproximation(Eigen::VectorXd& x)
{
	return approximations_queue_.try_pop(x);
}

void IterativeMethod::EnableFlipAvoidingLineSearch(Eigen::MatrixX3i& f)
{
	f_ = f;
	flip_avoiding_line_search_enabled_ = true;
}

void IterativeMethod::DisableFlipAvoidingLineSearch(Eigen::MatrixX3i& f)
{
	flip_avoiding_line_search_enabled_ = false;
}