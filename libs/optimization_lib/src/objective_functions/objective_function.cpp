// Optimization lib includes
#include <objective_functions/objective_function.h>

ObjectiveFunction::ObjectiveFunction(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::string& name) :
	f_(0),
	w_(1),
	name_(name),
	objective_function_data_provider_(objective_function_data_provider)
{

}

ObjectiveFunction::~ObjectiveFunction()
{
	// Empty implementation
}

void ObjectiveFunction::PreInitialize()
{
	domain_faces_count_ = objective_function_data_provider_->GetDomainFaces().rows();
	domain_vertices_count_ = objective_function_data_provider_->GetDomainVertices().rows();
	image_faces_count_ = objective_function_data_provider_->GetImageFaces().rows();
	image_vertices_count_ = objective_function_data_provider_->GetImageVerticesCount();
	variables_count_ = 2 * image_vertices_count_;
}

void ObjectiveFunction::Initialize()
{
	PreInitialize();
	InitializeGradient(g_);
	InitializeHessian(ii_, jj_, ss_);
	PostInitialize();
}

void ObjectiveFunction::PostInitialize()
{
	// Empty implementation
}

void ObjectiveFunction::InitializeGradient(Eigen::VectorXd& g)
{
	g.conservativeResize(variables_count_);
	g.setZero();
}

void ObjectiveFunction::PreUpdate(const Eigen::VectorXd& x)
{
	// Empty implementation
}

bool ObjectiveFunction::IsValid()
{
	return true;
}

void ObjectiveFunction::PostUpdate(const Eigen::VectorXd& x)
{
	// Empty implementation
}

const std::string ObjectiveFunction::GetName() const
{
	std::lock_guard<std::mutex> lock(m_);
	return name_;
}

void ObjectiveFunction::SetWeight(const double w)
{
	std::lock_guard<std::mutex> lock(m_);
	w_ = w;
}