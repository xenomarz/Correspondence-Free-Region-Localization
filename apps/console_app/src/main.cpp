// STL includes
#include <sstream>
#include <any>
#include <vector>
#include <chrono>
#include <thread>

// LIBIGL includes
#include <igl/readOFF.h>
#include <igl/readOBJ.h>

// Optimization lib includes
#include "../libs/optimization_lib/include/objective_functions/objective_function.h"
#include "../libs/optimization_lib/include/objective_functions/dense_objective_function.h"
#include "../libs/optimization_lib/include/objective_functions/position/barycenter_position_objective.h"
#include "../libs/optimization_lib/include/objective_functions/position/vertex_position_objective.h"
#include "../libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_angle_objective.h"
#include "../libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_length_objective.h"
#include "../libs/optimization_lib/include/objective_functions/singularity/singular_points_objective.h"
#include "../libs/optimization_lib/include/objective_functions/separation_objective.h"
#include "../libs/optimization_lib/include/objective_functions/symmetric_dirichlet_objective.h"
#include "../libs/optimization_lib/include/objective_functions/seamless_objective.h"
#include "../libs/optimization_lib/include/data_providers/mesh_wrapper.h"
#include "../libs/optimization_lib/include/solvers/pardiso_solver.h"
#include "../libs/optimization_lib/include/iterative_methods/newton_method.h"

void main()
{

	auto mesh_wrapper_ = std::make_shared<MeshWrapper>();
	auto empty_data_provider_ = std::make_shared<EmptyDataProvider>(mesh_wrapper_);
	auto plain_data_provider_ = std::make_shared<PlainDataProvider>(mesh_wrapper_);

	auto separation_ = std::make_shared<Separation<Eigen::StorageOptions::RowMajor>>(plain_data_provider_);
	auto symmetric_dirichlet_ = std::make_shared<SymmetricDirichlet<Eigen::StorageOptions::RowMajor>>(plain_data_provider_);
	auto seamless_ = std::make_shared<SeamlessObjective<Eigen::StorageOptions::RowMajor>>(empty_data_provider_);
	auto singular_points_ = std::make_shared<SingularPointsObjective<Eigen::StorageOptions::RowMajor>>(empty_data_provider_, 1);
	auto position_ = std::make_shared<SummationObjective<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>, Eigen::VectorXd>>(empty_data_provider_, std::string("Position"));
	std::vector<std::shared_ptr<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>>> objective_functions;
	//objective_functions.push_back(position_);
	objective_functions.push_back(separation_);
	objective_functions.push_back(symmetric_dirichlet_);
	//objective_functions.push_back(seamless_);
	//objective_functions.push_back(singular_points_);
	auto summation_objective_ = std::make_shared<SummationObjective<ObjectiveFunction<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>, Eigen::VectorXd>>(empty_data_provider_, objective_functions, false, false, true);

	std::shared_ptr<NewtonMethod<PardisoSolver, Eigen::StorageOptions::RowMajor>> newton_method_;
	mesh_wrapper_->RegisterModelLoadedCallback([&]() {
		/**
		 * Initialize objective functions
		 */
		summation_objective_->Initialize();

		/**
		 * Create newton method iterator
		 */
		auto image_vertices = mesh_wrapper_->GetImageVertices();
		auto x0 = Eigen::Map<const Eigen::VectorXd>(image_vertices.data(), image_vertices.cols() * image_vertices.rows());
		newton_method_ = std::make_unique<NewtonMethod<PardisoSolver, Eigen::StorageOptions::RowMajor>>(summation_objective_, x0);
		newton_method_->EnableFlipAvoidingLineSearch(mesh_wrapper_->GetImageFaces());
		newton_method_->Start();
	});

	mesh_wrapper_->LoadModel("C:\\Users\\Roy\\Documents\\GitHub\\RDS\\models\\obj\\cow.obj");

	std::this_thread::sleep_for(std::chrono::milliseconds(20000));
}
