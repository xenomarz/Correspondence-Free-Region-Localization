#pragma once
#ifndef OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H
#define OPTIMIZATION_LIB_SEAMLESS_OBJECTIVE_H

// C includes
#define _USE_MATH_DEFINES
#include <math.h>

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "../data_providers/empty_data_provider.h"
#include "../data_providers/edge_pair_data_provider.h"
#include "./summation_objective.h"
#include "./edge_pair/edge_pair_objective.h"
#include "./edge_pair/edge_pair_angle_objective.h"
#include "./edge_pair/edge_pair_length_objective.h"
#include "../objective_functions/periodic_objective.h"

template <Eigen::StorageOptions StorageOrder_>
class SeamlessObjective : public SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>
{
public:
	/**
	 * Public type definitions
	 */
	enum class Properties : int32_t
	{
		Zeta = SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::Properties::Count_
	};
	
	/**
	 * Constructors and destructor
	 */
	SeamlessObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const std::string& name, const bool enforce_children_psd = true) :
		SummationObjective(mesh_data_provider, empty_data_provider, name, enforce_children_psd),
		zeta_(1)
	{

	}

	SeamlessObjective(const std::shared_ptr<MeshDataProvider>& mesh_data_provider, const std::shared_ptr<EmptyDataProvider>& empty_data_provider, const bool enforce_children_psd = true) :
		SeamlessObjective(mesh_data_provider, empty_data_provider, "Seamless", enforce_children_psd)
	{

	}

	virtual ~SeamlessObjective()
	{

	}

	/**
	 * Setters
	 */
	void SetZeta(const double zeta)
	{
		for(auto& periodic_edge_pair_angle_objective : periodic_edge_pair_angle_objectives)
		{
			periodic_edge_pair_angle_objective->SetWeight(1.0 - zeta);
			//edge_pair_angle_objective->SetWeight(1.0 - zeta);
		}

		for (auto& edge_pair_length_objective : edge_pair_length_objectives)
		{
			edge_pair_length_objective->SetWeight(zeta);
		}
		
		zeta_ = zeta;
	}

	bool SetProperty(const int32_t property_id, const std::any& property_value) override
	{
		if (SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::SetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Zeta:
			SetZeta(std::any_cast<const double>(property_value));
			return true;
		}

		return false;
	}

	/**
	 * Getters
	 */
	double GetZeta() const
	{
		return zeta_;
	}

	bool GetProperty(const int32_t property_id, std::any& property_value) override
	{
		if (SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::GetProperty(property_id, property_value))
		{
			return true;
		}

		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Zeta:
			property_value = GetZeta();
			return true;
		}

		return false;
	}

	/**
	 * Public methods
	 */
	void AddEdgePairObjectives(const std::shared_ptr<EdgePairDataProvider>& edge_pair_data_provider)
	{	
		std::shared_ptr<EdgePairAngleObjective<StorageOrder_>> edge_pair_angle_objective = std::make_shared<EdgePairAngleObjective<StorageOrder_>>(this->GetMeshDataProvider(), edge_pair_data_provider, false);
		std::shared_ptr<EdgePairLengthObjective<StorageOrder_>> edge_pair_length_objective = std::make_shared<EdgePairLengthObjective<StorageOrder_>>(this->GetMeshDataProvider(), edge_pair_data_provider, false);

		double period = M_PI / 2;
		auto empty_data_provider = std::make_shared<EmptyDataProvider>(this->GetMeshDataProvider());
		std::shared_ptr<PeriodicObjective<StorageOrder_>> periodic_edge_pair_angle_objective = std::make_shared<PeriodicObjective<StorageOrder_>>(this->GetMeshDataProvider(), empty_data_provider, edge_pair_angle_objective, period, this->GetEnforceChildrenPsd());

		this->AddObjectiveFunction(periodic_edge_pair_angle_objective);
		this->AddObjectiveFunction(edge_pair_length_objective);

		periodic_edge_pair_angle_objectives.push_back(periodic_edge_pair_angle_objective);
		edge_pair_length_objectives.push_back(edge_pair_length_objective);
		edge_pair_angle_objectives.push_back(edge_pair_angle_objective);
	}

	/**
	 * Fields
	 */
	double zeta_;
	std::vector<std::shared_ptr<EdgePairLengthObjective<StorageOrder_>>> edge_pair_length_objectives;
	std::vector<std::shared_ptr<EdgePairAngleObjective<StorageOrder_>>> edge_pair_angle_objectives;
	std::vector<std::shared_ptr<PeriodicObjective<StorageOrder_>>> periodic_edge_pair_angle_objectives;
};

#endif