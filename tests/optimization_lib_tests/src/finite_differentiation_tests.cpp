// GTest includes
#include <gtest/gtest.h>

// STL includes
#include <memory>
#include <filesystem>

// Optimization lib includes
#include <libs/optimization_lib/include/utils/mesh_wrapper.h>
#include <libs/optimization_lib/include/utils/utils.h>
#include <libs/optimization_lib/include/utils/data_providers/data_provider.h>
#include <libs/optimization_lib/include/utils/data_providers/plain_data_provider.h>
#include <libs/optimization_lib/include/utils/data_providers/edge_pair_data_provider.h>
#include <libs/optimization_lib/include/utils/data_providers/adjacent_faces_data_provider.h>

#include <libs/optimization_lib/include/objective_functions/objective_function.h>
#include <libs/optimization_lib/include/objective_functions/composite_objective.h>
#include <libs/optimization_lib/include/objective_functions/edge_pair/edge_pair_angle_objective.h>
#include <libs/optimization_lib/include/objective_functions/coordinate_objective.h>
#include <libs/optimization_lib/include/objective_functions/periodic_objective.h>

template<Eigen::StorageOptions StorageOrder_, typename VectorType_>
class FiniteDifferencesTest : public ::testing::Test
{
protected:
	FiniteDifferencesTest(const std::string& filename) :
		filename_(filename)
	{
		mesh_wrapper_ = std::make_shared<MeshWrapper>();
	}

	virtual ~FiniteDifferencesTest() override
	{

	}

	void SetUp() override
	{
		mesh_wrapper_->RegisterModelLoadedCallback([this]() {
			CreateDataProvider();
			CreateObjectiveFunction();
			auto image_vertices = mesh_wrapper_->GetImageVertices();
			x_ = Eigen::Map<const Eigen::VectorXd>(image_vertices.data(), image_vertices.cols() * image_vertices.rows());
		});

		mesh_wrapper_->LoadModel(filename_);
	}

	void TearDown() override
	{

	}

	virtual void CreateObjectiveFunction() = 0;
	
	virtual void CreateDataProvider() = 0;

	void AssertComponent(const double value, const double approximation) const
	{
		const double absolute_error = std::abs(value - approximation);
		const double relative_error = std::abs(absolute_error / (value + std::numeric_limits<double>::epsilon()));
		ASSERT_LT(relative_error, 1e-4);
	}

	void AssertGradient() const
	{
		objective_function_->Update(x_);
		Eigen::VectorXd analytic_g = objective_function_->GetGradient();
		Eigen::VectorXd approx_g = ObjectiveFunction<StorageOrder_, VectorType_>::GetApproximatedGradient(objective_function_, x_);

		for (uint64_t i = 0; i < analytic_g.rows(); i++)
		{
			AssertComponent(analytic_g.coeffRef(i), approx_g.coeffRef(i));
		}
	}

	void AssertHessian(bool symmetric = false) const
	{
		objective_function_->Update(x_);
		Eigen::MatrixXd analytic_H = objective_function_->GetHessian();
		Eigen::MatrixXd approx_H = ObjectiveFunction<StorageOrder_, VectorType_>::GetApproximatedHessian(objective_function_, x_);

		for (uint64_t row = 0; row < analytic_H.rows(); row++)
		{
			for (uint64_t col = 0; col < analytic_H.cols(); col++)
			{
				if((symmetric && col >= row) || !symmetric)
				{
					AssertComponent(analytic_H.coeffRef(row, col), approx_H.coeffRef(row, col));
				}
			}
		}
	}

	std::shared_ptr<ObjectiveFunction<StorageOrder_, VectorType_>> objective_function_;
	std::shared_ptr<MeshWrapper> mesh_wrapper_;
	std::shared_ptr<DataProvider> data_provider_;
	Eigen::VectorXd x_;
	std::string filename_;
};

class PeriodicEdgePairAngleObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
{
protected:
	PeriodicEdgePairAngleObjectiveFDTest() :
		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
	{

	}

	~PeriodicEdgePairAngleObjectiveFDTest() override
	{

	}

	void CreateDataProvider() override
	{
		auto& edge_pair_descriptors = mesh_wrapper_->GetEdgePairDescriptors();
		data_provider_ = std::make_shared<EdgePairDataProvider>(mesh_wrapper_, edge_pair_descriptors[0]);
	}

	void CreateObjectiveFunction() override
	{
		auto edge_pair_length_objective = std::make_shared<EdgePairAngleObjective<Eigen::StorageOptions::RowMajor>>(std::dynamic_pointer_cast<EdgePairDataProvider>(data_provider_));	
		objective_function_ = std::make_shared<PeriodicObjective<Eigen::StorageOptions::RowMajor>>(true, edge_pair_length_objective, 2 * M_PI);
	}
};

class PeriodicCoordinateObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
{
protected:
	PeriodicCoordinateObjectiveFDTest() :
		FiniteDifferencesTest("../../../models/obj/two_triangles_v2.obj")
	{

	}

	~PeriodicCoordinateObjectiveFDTest() override
	{

	}

	void CreateDataProvider() override
	{
		data_provider_ = std::make_shared<PlainDataProvider>(mesh_wrapper_);
	}

	void CreateObjectiveFunction() override
	{
		auto coordinate_objective = std::make_shared<CoordinateObjective<Eigen::StorageOptions::RowMajor>>(std::dynamic_pointer_cast<PlainDataProvider>(data_provider_), 1, CoordinateObjective<Eigen::StorageOptions::RowMajor>::CoordinateType::X);
		objective_function_ = std::make_shared<PeriodicObjective<Eigen::StorageOptions::RowMajor>>(false, coordinate_objective, 1);
	}
};

//class SeamlessObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>
//{
//protected:
//	SeamlessObjectiveFDTest() :
//		FiniteDifferencesTest("../../models/obj/two_triangles_v2.obj")
//	{
//
//	}
//
//	~SeamlessObjectiveFDTest() override
//	{
//
//	}
//
//	void CreateObjectiveFunction() override
//	{
//		auto corresponding_edge_pairs = mesh_wrapper_->GetEdgePairDescriptors();
//		auto seamless_objective = std::make_shared<SeamlessObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, false);
//		//seamless_objective->AddCorrespondingEdgePairs(corresponding_edge_pairs);
//		objective_function_ = seamless_objective;
//	}
//};

TEST_F(PeriodicEdgePairAngleObjectiveFDTest, Gradient)
{
	AssertGradient();
}

TEST_F(PeriodicEdgePairAngleObjectiveFDTest, Hessian)
{
	AssertHessian(true);
}

TEST_F(PeriodicCoordinateObjectiveFDTest, Gradient)
{
	AssertGradient();
}

TEST_F(PeriodicCoordinateObjectiveFDTest, Hessian)
{
	AssertHessian(true);
}

//TEST_F(SeamlessObjectiveFDTest, Gradient)
//{
//	AssertGradient();
//}
//
//TEST_F(SeamlessObjectiveFDTest, Hessian)
//{
//	AssertHessian(true);
//}
