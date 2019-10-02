// Optimization library includes
#include <utils/objective_function_data_provider.h>

void ObjectiveFunctionDataProvider::Subscribe(const std::string& event_name, const ObjectiveFunctionDataProviderEventHandler& event_handler)
{
	if (event_handlers.contains(event_name))
	{
		event_handlers[event_name].push_back(event_handler);
	}
	else
	{
		event_handlers[event_name] = std::vector<ObjectiveFunctionDataProviderEventHandler>{ event_handler };
	}
}

void ObjectiveFunctionDataProvider::Publish(const std::string& event_name)
{
	if (event_handlers.contains(event_name))
	{
		for (auto& event_handler : event_handlers[event_name])
		{
			event_handler(event_name);
		}
	}
}