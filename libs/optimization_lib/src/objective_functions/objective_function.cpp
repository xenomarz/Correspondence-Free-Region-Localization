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

void ObjectiveFunction::Update(const Eigen::MatrixX2d& x)
{
	CalculateValue(x, f_);
	CalculateGradient(x, g_);
	CalculateHessian(x, ss_);
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
	return ii_;
}

const std::vector<int>& ObjectiveFunction::GetJJ() const
{
	return jj_;
}

const std::vector<double>& ObjectiveFunction::GetSS() const
{
	return ss_;
}

const MeshWrapper& ObjectiveFunction::GetMeshWrapper() const
{
	return *mesh_wrapper_;
}
