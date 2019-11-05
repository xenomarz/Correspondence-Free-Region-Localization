// GTest includes
#include <gtest/gtest.h>

// STL includes
#include <memory>
#include <filesystem>

// Optimization lib includes
#include <libs/optimization_lib/include/utils/mesh_wrapper.h>
#include <libs/optimization_lib/include/objective_functions/edge_pair_angle_objective.h>
#include <libs/optimization_lib/include/objective_functions/integer_objective.h>
#include <libs/optimization_lib/include/objective_functions/seamless_objective.h>
#include <libs/optimization_lib/include/utils/utils.h>

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
		Eigen::VectorXd approx_g = Utils::GetApproximatedGradient<StorageOrder_, VectorType_>(objective_function_, x_);

		for (uint64_t i = 0; i < analytic_g.rows(); i++)
		{
			AssertComponent(analytic_g.coeffRef(i), approx_g.coeffRef(i));
		}
	}

	void AssertHessian(bool symmetric = false) const
	{
		objective_function_->Update(x_);
		Eigen::MatrixXd analytic_H = objective_function_->GetHessian();
		Eigen::MatrixXd approx_H = Utils::GetApproximatedHessian<StorageOrder_, VectorType_>(objective_function_, x_);

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
	Eigen::VectorXd x_;
	std::string filename_;
};

class EdgePairAngleObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
{
protected:
	EdgePairAngleObjectiveFDTest() :
		FiniteDifferencesTest("../../models/obj/two_triangles_v2.obj")
	{

	}

	~EdgePairAngleObjectiveFDTest() override
	{

	}

	void CreateObjectiveFunction() override
	{
		auto corresponding_edge_pair = mesh_wrapper_->GetCorrespondingEdgeVertices()[0];
		objective_function_ = std::make_shared<EdgePairAngleObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, corresponding_edge_pair.first, corresponding_edge_pair.second, false);
	}
};

class IntegerObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::SparseVector<double>>
{
protected:
	IntegerObjectiveFDTest() :
		FiniteDifferencesTest("../../models/obj/two_triangles_v2.obj")
	{

	}

	~IntegerObjectiveFDTest() override
	{

	}

	void CreateObjectiveFunction() override
	{
		objective_function_ = std::make_shared<IntegerObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, 9, 1);
	}
};

class SeamlessObjectiveFDTest : public FiniteDifferencesTest<Eigen::StorageOptions::RowMajor, Eigen::VectorXd>
{
protected:
	SeamlessObjectiveFDTest() :
		FiniteDifferencesTest("../../models/obj/two_triangles_v2.obj")
	{

	}

	~SeamlessObjectiveFDTest() override
	{

	}

	void CreateObjectiveFunction() override
	{
		auto corresponding_edge_pairs = mesh_wrapper_->GetCorrespondingEdgeVertices();
		auto seamless_objective = std::make_shared<SeamlessObjective<Eigen::StorageOptions::RowMajor>>(mesh_wrapper_, false);
		seamless_objective->AddCorrespondingEdgePairs(corresponding_edge_pairs);
		objective_function_ = seamless_objective;
	}
};

TEST_F(EdgePairAngleObjectiveFDTest, Gradient)
{
	AssertGradient();
}

TEST_F(EdgePairAngleObjectiveFDTest, Hessian)
{
	AssertHessian(true);
}

TEST_F(IntegerObjectiveFDTest, Gradient)
{
	AssertGradient();
}

TEST_F(IntegerObjectiveFDTest, Hessian)
{
	AssertHessian(true);
}

TEST_F(SeamlessObjectiveFDTest, Gradient)
{
	AssertGradient();
}

TEST_F(SeamlessObjectiveFDTest, Hessian)
{
	AssertHessian(true);
}