#include <objective_functions/objective_function.h>

ObjectiveFunction::ObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
	f_(0),
	w_(1),
	name_(name),
	objective_function_data_provider_(objective_function_data_provider),
	domain_faces_count_(objective_function_data_provider->GetDomainFaces().rows()),
	domain_vertices_count_(objective_function_data_provider->GetDomainVertices().rows()),
	image_faces_count_(objective_function_data_provider->GetImageFaces().rows()),
	image_vertices_count_(objective_function_data_provider->GetImageVerticesCount()),
	variables_count_(2 * image_vertices_count_)
{

}


ObjectiveFunction::~ObjectiveFunction()
{

}

void ObjectiveFunction::InitializeGradient(Eigen::VectorXd& g)
{
	g.conservativeResize(objective_function_data_provider_->GetImageVerticesCount());
}

void ObjectiveFunction::PreUpdate(const Eigen::MatrixX2d& x)
{

}

void ObjectiveFunction::Update(const Eigen::MatrixX2d& x)
{
	std::unique_lock<std::mutex> lock(m_);
	PreUpdate(x);
	CalculateValue(x, f_);
	CalculateGradient(x, g_);
	CalculateHessian(x, ss_);
	CalculateHessianInternal(ii_, jj_, ss_, H_);
	PostUpdate(x);
}

void ObjectiveFunction::PostUpdate(const Eigen::MatrixX2d& x)
{

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

double ObjectiveFunction::GetWeight() const
{
	return w_;
}

const std::string ObjectiveFunction::GetName() const
{
	return name_;
}

void ObjectiveFunction::SetWeight(const double w)
{
	w_ = w;
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