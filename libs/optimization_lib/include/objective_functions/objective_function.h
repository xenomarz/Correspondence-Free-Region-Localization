#pragma once

#include <Eigen/Core>
#include <vector>

class ObjectiveFunction
{
public:
	ObjectiveFunction();
	virtual ~ObjectiveFunction();

	// Value, Gradient and Hessian getters
	double GetValue() const;
	const Eigen::VectorXd& GetGradient() const;
	const std::vector<int>& GetII() const;
	const std::vector<int>& GetJJ() const;
	const std::vector<double>& GetSS() const;

	// Update Value, Gradient and Hessian for the given x
	virtual void Update(const Eigen::VectorXd& x) = 0;

protected:
	virtual void PrepareHessian() = 0;

	// Value
	double f;

	// Gradient
	Eigen::VectorXd g;

	// Hessian (sparse reprensentation)
	std::vector<int> II, JJ;
	std::vector<double> SS;
};

