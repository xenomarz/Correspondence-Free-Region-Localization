#include <objective_functions/position.h>

Position::Position(const std::shared_ptr<MeshWrapper>& mesh_wrapper) :
	ObjectiveFunction(mesh_wrapper, "Position")
{

}

Position::~Position()
{

}

void Position::InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& ii, std::vector<int>& jj, std::vector<double>& ss)
{

}

void Position::CalculateValue(const Eigen::MatrixX2d& X, double& f)
{

}

void Position::CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g)
{

}

void Position::CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& ss)
{

}