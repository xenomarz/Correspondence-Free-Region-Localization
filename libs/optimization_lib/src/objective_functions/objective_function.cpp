#include "objective_functions/objective_function.h"

ObjectiveFunction::ObjectiveFunction(const std::shared_ptr<MeshWrapper>& mesh_wrapper) :
	f_(0),
	mesh_wrapper_(mesh_wrapper)
{

}


ObjectiveFunction::~ObjectiveFunction()
{

}

void ObjectiveFunction::InitializeGradient(const std::shared_ptr<MeshWrapper>& mesh_wrapper, Eigen::VectorXd& g)
{
	g.conservativeResize(mesh_wrapper->GetImageVertices().rows());
}

void ObjectiveFunction::Update(const Eigen::MatrixX2d& X)
{
	CalculateValue(X, f_);
	CalculateGradient(X, g_);
	CalculateHessian(X, SS_);
}

double ObjectiveFunction::GetValue() const
{
	return f_;
}

const Eigen::VectorXd& ObjectiveFunction::GetGradient() const
{
	return g_;
}

const std::vector<int>& ObjectiveFunction::GetII() const
{
	return II_;
}

const std::vector<int>& ObjectiveFunction::GetJJ() const
{
	return JJ_;
}

const std::vector<double>& ObjectiveFunction::GetSS() const
{
	return SS_;
}

const MeshWrapper& ObjectiveFunction::GetMeshWrapper() const
{
	return *mesh_wrapper_;
}
