// Optimization lib includes
#include <objective_functions/objective_function.h>
#include <utils/utils.h>

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
	// Empty implementation
}

void ObjectiveFunction::PreInitialize()
{
	// Empty implementation
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
	g.conservativeResize(image_vertices_count_);
}

void ObjectiveFunction::PreUpdate(const Eigen::VectorXd& x)
{
	// Empty implementation
}

void ObjectiveFunction::Update(const Eigen::VectorXd& x)
{
	std::lock_guard<std::mutex> lock(m_);
	if (IsValid())
	{
		PreUpdate(x);
		CalculateValue(x, f_);
		CalculateGradient(x, g_);
		CalculateHessian(x, ss_);
		Utils::SparseMatrixFromTriplets(ii_, jj_, ss_, image_vertices_count_, image_vertices_count_, H_);
		PostUpdate(x);
	}
}

bool ObjectiveFunction::IsValid()
{
	return true;
}

void ObjectiveFunction::PostUpdate(const Eigen::VectorXd& x)
{
	// Empty implementation
}

double ObjectiveFunction::GetValue() const
{
	std::lock_guard<std::mutex> lock(m_);
	return f_;
}

const Eigen::VectorXd& ObjectiveFunction::GetGradient() const
{
	std::lock_guard<std::mutex> lock(m_);
	return g_;
}

const std::vector<int>& ObjectiveFunction::GetII() const
{
	std::lock_guard<std::mutex> lock(m_);
	return ii_;
}

const std::vector<int>& ObjectiveFunction::GetJJ() const
{
	std::lock_guard<std::mutex> lock(m_);
	return jj_;
}

const std::vector<double>& ObjectiveFunction::GetSS() const
{
	std::lock_guard<std::mutex> lock(m_);
	return ss_;
}

const Eigen::SparseMatrix<double>& ObjectiveFunction::GetHessian() const
{
	std::lock_guard<std::mutex> lock(m_);
	return H_;
}

double ObjectiveFunction::GetWeight() const
{
	std::lock_guard<std::mutex> lock(m_);
	return w_;
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