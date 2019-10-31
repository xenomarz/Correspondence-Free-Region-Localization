// GTest includes
#include <gtest/gtest.h>

// STL includes
#include <memory>

// Optimization lib includes
#include <libs/optimization_lib/include/utils/mesh_wrapper.h>
#include <libs/optimization_lib/include/objective_functions/edge_pair_angle_objective.h>
#include <libs/optimization_lib/include/utils/utils.h>
TEST(FiniteDifferences_EdgePairAngleObjective, Gradient)
{
	auto mesh_wrapper = std::make_shared<MeshWrapper>();
	mesh_wrapper->RegisterModelLoadedCallback([&]() {

	});
}