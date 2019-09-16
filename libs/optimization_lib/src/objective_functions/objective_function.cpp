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
	CalculateHessianInternal(ii_, jj_, ss_, H_);
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

const Eigen::SparseMatrix<double>& ObjectiveFunction::GetHessian() const
{
	return H_;
}

const MeshWrapper& ObjectiveFunction::GetMeshWrapper() const
{
	return *mesh_wrapper_;
}

void ObjectiveFunction::CalculateHessianInternal(const std::vector<int>& ii, const std::vector<int>& jj, const std::vector<double>& ss, Eigen::SparseMatrix<double>& H)
{
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(ii.size());
	int rows = *std::max_element(ii.begin(), ii.end()) + 1;
	int cols = *std::max_element(jj.begin(), jj.end()) + 1;

	for (int i = 0; i < ii_.size(); i++)
	{
		triplets.push_back(Eigen::Triplet<double>(ii[i], jj[i], ss[i]));
	}

	H_.resize(rows, cols);
	H_.setFromTriplets(triplets.begin(), triplets.end());
}