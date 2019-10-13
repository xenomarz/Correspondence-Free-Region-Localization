// Optimization lib includes
#include <objective_functions/position_objective.h>

PositionObjective::PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double coefficient, const int64_t vertices_count, const std::string& name) :
	ObjectiveFunction(objective_function_data_provider, name),
	objective_vertices_count_(vertices_count),
	objective_variables_count_(2 * vertices_count),
	coefficient_(coefficient)
{

}

PositionObjective::PositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const double coefficient, const int64_t vertices_count) :
	PositionObjective(objective_function_data_provider, coefficient, vertices_count, "Position Objective")
{

}

PositionObjective::~PositionObjective()
{

}