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
	InitializeValue(f_, f_per_vertex_);
	InitializeGradient(g_);
	InitializeHessian(ii_, jj_, ss_);
	PostInitialize();
}

void ObjectiveFunction::PostInitialize()
{
	// Empty implementation
}

void ObjectiveFunction::InitializeValue(double& f, Eigen::VectorXd& f_per_vertex)
{
	f = 0;
	f_per_vertex.conservativeResize(image_vertices_count_);
	f_per_vertex.setZero();
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

void ObjectiveFunction::Update(const Eigen::VectorXd& x, const UpdateOptions update_options)
{
	std::lock_guard<std::mutex> lock(m_);
	PreUpdate(x);

	if ((update_options & UpdateOptions::VALUE) != UpdateOptions::NONE)
	{
		CalculateValue(f_, f_per_vertex_);
	}

	if ((update_options & UpdateOptions::GRADIENT) != UpdateOptions::NONE)
	{
		CalculateGradient(g_);
	}

	if ((update_options & UpdateOptions::HESSIAN) != UpdateOptions::NONE)
	{
		CalculateHessian(ss_);
		//Utils::SparseMatrixFromTriplets(ii_, jj_, ss_, variables_count_, variables_count_, H_cm_);
		Utils::SparseMatrixFromTriplets(ii_, jj_, ss_, variables_count_, variables_count_, H_rm_);
	}

	PostUpdate(x);
}