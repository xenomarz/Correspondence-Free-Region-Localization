//// GTest includes
//#include <gtest/gtest.h>
//
//// STL includes
//#include <memory>
//#include <filesystem>
//
//// Optimization lib includes
//#include <libs/optimization_lib/include/core/utils.h>
//#include <libs/optimization_lib/include/data_providers/mesh_wrapper.h>
//#include <libs/optimization_lib/include/data_providers/data_provider.h>
//#include <libs/optimization_lib/include/data_providers/empty_data_provider.h>
//#include <libs/optimization_lib/include/data_providers/plain_data_provider.h>
//#include <libs/optimization_lib/include/data_providers/coordinate_data_provider.h>
//#include <libs/optimization_lib/include/data_providers/edge_pair_data_provider.h>
//#include <libs/optimization_lib/include/data_providers/face_fan_data_provider.h>
//#include <libs/optimization_lib/include/objective_functions/objective_function.h>
//#include <libs/optimization_lib/include/objective_functions/composite_objective.h>
//#include <libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_angle_objective.h>
//#include <libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_length_objective.h>
//#include <libs/optimization_lib/include/objective_functions/coordinate_objective.h>
//#include <libs/optimization_lib/include/objective_functions/periodic_objective.h>
//#include <libs/optimization_lib/include/objective_functions/singularity/singular_point_objective.h>
//#include <libs/optimization_lib/include/objective_functions/singularity/singular_points_objective.h>
//#include <libs/optimization_lib/include/objective_functions/seamless_objective.h>
//#include <libs/optimization_lib/include/objective_functions/separation_objective.h>
//
//template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
//class FiniteDifferencesTest : public ::testing::Test
//{
//protected:
//	FiniteDifferencesTest(const std::string& filename) :
//		filename_(filename)
//	{
//		mesh_wrapper_ = std::make_shared<MeshWrapper>();
//	}
//
//	virtual ~FiniteDifferencesTest() override
//	{
//
//	}
//
//	void SetUp() override
//	{
//		mesh_wrapper_->RegisterModelLoadedCallback([this]() {
//			CreateDataProvider();
//			CreateObjectiveFunction();
//			auto image_vertices = mesh_wrapper_->GetImageVertices();
//			x_ = Eigen::Map<const Eigen::VectorXd>(image_vertices.data(), image_vertices.cols() * image_vertices.rows());
//		});
//
//		mesh_wrapper_->LoadModel(filename_);
//	}
//
//	void TearDown() override
//	{
//
//	}
//
//	virtual void CreateObjectiveFunction() = 0;
//	
//	virtual void CreateDataProvider() = 0;
//
//	void AssertComponent(const double value, const double approximation) const
//	{
//		const double absolute_error = std::abs(value - approximation);
//		const double relative_error = std::abs(absolute_error / (value + std::numeric_limits<double>::epsilon()));
//		ASSERT_LT(relative_error, 1e-4);
//	}
//
//	void AssertGradient() const
//	{
//		objective_function_->Update(x_);
//		Eigen::VectorXd analytic_g = objective_function_->GetGradient();
//		Eigen::VectorXd approx_g = ObjectiveFunction<StorageOrder_, VectorType_>::GetApproximatedGradient(objective_function_, x_);
//
//		for (uint64_t i = 0; i < analytic_g.rows(); i++)
//		{
//			AssertComponent(analytic_g.coeffRef(i), approx_g.coeffRef(i));
//		}
//	}
//
//	void AssertHessian(bool symmetric = false) const
//	{
//		objective_function_->Update(x_);
//		Eigen::MatrixXd analytic_H = objective_function_->GetHessian();
//		Eigen::MatrixXd approx_H = ObjectiveFunction<StorageOrder_, VectorType_>::GetApproximatedHessian(objective_function_, x_);
//
//		for (uint64_t row = 0; row < analytic_H.rows(); row++)
//		{
//			for (uint64_t col = 0; col < analytic_H.cols(); col++)
//			{
//				if((symmetric && col >= row) || !symmetric)
//				{
//					AssertComponent(analytic_H.coeffRef(row, col), approx_H.coeffRef(row, col));
//				}
//			}
//		}
//	}
//
//	std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>> objective_function_;
//	std::shared_ptr<MeshWrapper> mesh_wrapper_;
//	std::vector<std::shared_ptr<DataProvider>> data_providers_;
//	Eigen::VectorXd x_;
//	std::string filename_;
//};
//
//class PeriodicEdgePairAngleObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
//{
//protected:
//	PeriodicEdgePairAngleObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~PeriodicEdgePairAngleObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		auto& edge_pair_descriptors = mesh_wrapper_->GetEdgePairDescriptors();
//		data_providers_.push_back(std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptors[0]));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		auto edge_pair_length_objective = std::make_shared<EdgePairAngleObjective<Eigen::StorageOptions::RowMajor>>(std::dynamic_pointer_cast<EdgePairDataProvider>(data_providers_[0]));	
//		objective_function_ = std::make_shared<PeriodicObjective<Eigen::StorageOptions::RowMajor>>(edge_pair_length_objective, M_PI / 2, false);
//	}
//};
//
//class PeriodicCoordinateObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
//{
//protected:
//	PeriodicCoordinateObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~PeriodicCoordinateObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		data_providers_.push_back(std::make_shared<CoordinateDataProvider>(mesh_wrapper_, 1, CoordinateDataProvider::CoordinateType::Y));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		auto coordinate_objective = std::make_shared<CoordinateObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, std::static_pointer_cast<CoordinateDataProvider>(data_providers_[0]));
//		objective_function_ = std::make_shared<PeriodicObjective<Eigen::StorageOptions::RowMajor>>(coordinate_objective, 1, false);
//	}
//};
//
//class EdgePairLengthObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
//{
//protected:
//	EdgePairLengthObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~EdgePairLengthObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		auto& edge_pair_descriptors = mesh_wrapper_->GetEdgePairDescriptors();
//		data_providers_.push_back(std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptors[0]));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		objective_function_ = std::make_shared<EdgePairLengthObjective<Eigen::StorageOptions::RowMajor>>(std::dynamic_pointer_cast<EdgePairDataProvider>(data_providers_[0]), false);
//	}
//};
//
//class SingularPointObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
//{
//protected:
//	SingularPointObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~SingularPointObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		auto& face_fans = mesh_wrapper_->GetFaceFans();
//		data_providers_.push_back(std::make_shared<FaceFanDataProvider>(mesh_wrapper_, face_fans[1]));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		objective_function_ = std::make_shared<SingularPointObjective<Eigen::StorageOptions::RowMajor>>(std::dynamic_pointer_cast<FaceFanDataProvider>(data_providers_[0]), 1, false);
//	}
//};
//
//class SingularPointsObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>
//{
//protected:
//	SingularPointsObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~SingularPointsObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		data_providers_.push_back(std::make_shared<EmptyDataProvider>(mesh_wrapper_));
//		
//		auto& face_fans = mesh_wrapper_->GetFaceFans();	
//		data_providers_.push_back(std::make_shared<FaceFanDataProvider>(mesh_wrapper_, face_fans[1]));
//		data_providers_.push_back(std::make_shared<FaceFanDataProvider>(mesh_wrapper_, face_fans[2]));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		std::shared_ptr<SingularPointsObjective<Eigen::StorageOptions::RowMajor>> singular_points_objective = std::make_shared<SingularPointsObjective<Eigen::StorageOptions::RowMajor>>(std::static_pointer_cast<EmptyDataProvider>(data_providers_[0]), 1, false);
//		singular_points_objective->AddSingularPointObjective(std::static_pointer_cast<FaceFanDataProvider>(data_providers_[1]));
//		singular_points_objective->AddSingularPointObjective(std::static_pointer_cast<FaceFanDataProvider>(data_providers_[2]));
//		objective_function_ = singular_points_objective;
//	}
//};
//
//class SeamlessObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>
//{
//protected:
//	SeamlessObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/three_triangles.obj")
//	{
//
//	}
//
//	~SeamlessObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		data_providers_.push_back(std::make_shared<EmptyDataProvider>(mesh_wrapper_));
//
//		auto& edge_pair_descriptors = mesh_wrapper_->GetEdgePairDescriptors();
//		data_providers_.push_back(std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptors[0]));
//		data_providers_.push_back(std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptors[1]));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		auto seamless_objective = std::make_shared<SeamlessObjective<Eigen::StorageOptions::RowMajor>>(std::static_pointer_cast<EmptyDataProvider>(data_providers_[0]), false);
//		seamless_objective->AddEdgePairObjectives(std::static_pointer_cast<EdgePairDataProvider>(data_providers_[1]));
//		seamless_objective->AddEdgePairObjectives(std::static_pointer_cast<EdgePairDataProvider>(data_providers_[2]));
//		objective_function_ = seamless_objective;
//	}
//};
//
//class SeparationObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>
//{
//protected:
//	SeparationObjectiveFDTest() :
//		FiniteDifferencesTest("../../../models/obj/three_triangles.obj")
//	{
//
//	}
//
//	~SeparationObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateDataProvider() override
//	{
//		data_providers_.push_back(std::make_shared<PlainDataProvider>(mesh_wrapper_));
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		const auto separation_objective = std::make_shared<Separation<Eigen::StorageOptions::RowMajor>>(std::static_pointer_cast<PlainDataProvider>(data_providers_[0]));
//		objective_function_ = separation_objective;
//	}
//};
//
//TEST_F(PeriodicEdgePairAngleObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(PeriodicEdgePairAngleObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(PeriodicCoordinateObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(PeriodicCoordinateObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(EdgePairLengthObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(EdgePairLengthObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(SingularPointObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(SingularPointObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(SingularPointsObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(SingularPointsObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(SeamlessObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(SeamlessObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
//
//TEST_F(SeparationObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(SeparationObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}