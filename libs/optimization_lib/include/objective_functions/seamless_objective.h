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
		Zeta = SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::Properties::Count_,
		AngleValuePerEdge,
		LengthValuePerEdge,
		ValuePerEdge
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

	const Eigen::VectorXd& SeamlessObjective::GetValuePerEdge(const ObjectiveFunctionBase::PropertyModifiers property_modifiers) const
	{
		switch(property_modifiers)
		{
		case ObjectiveFunctionBase::PropertyModifiers::Domain:
			return GetDomainValuePerEdge();
		case ObjectiveFunctionBase::PropertyModifiers::Image:
			return GetImageValuePerEdge();
		}
	}

	const Eigen::VectorXd& SeamlessObjective::GetAngleValuePerEdge(const ObjectiveFunctionBase::PropertyModifiers property_modifiers) const
	{
		switch (property_modifiers)
		{
		case ObjectiveFunctionBase::PropertyModifiers::Domain:
			return GetDomainAngleValuePerEdge();
		case ObjectiveFunctionBase::PropertyModifiers::Image:
			return GetImageAngleValuePerEdge();
		}
	}

	const Eigen::VectorXd& SeamlessObjective::GetLengthValuePerEdge(const ObjectiveFunctionBase::PropertyModifiers property_modifiers) const
	{
		switch (property_modifiers)
		{
		case ObjectiveFunctionBase::PropertyModifiers::Domain:
			return GetDomainLengthValuePerEdge();
		case ObjectiveFunctionBase::PropertyModifiers::Image:
			return GetImageLengthValuePerEdge();
		}
	}

	const Eigen::VectorXd& SeamlessObjective::GetImageValuePerEdge() const
	{
		return image_value_per_edge_;
	}

	const Eigen::VectorXd& SeamlessObjective::GetImageAngleValuePerEdge() const
	{
		return image_angle_value_per_edge_;
	}

	const Eigen::VectorXd& SeamlessObjective::GetImageLengthValuePerEdge() const
	{
		return image_length_value_per_edge_;
	}

	const Eigen::VectorXd& SeamlessObjective::GetDomainValuePerEdge() const
	{
		return domain_value_per_edge_;
	}

	const Eigen::VectorXd& SeamlessObjective::GetDomainAngleValuePerEdge() const
	{
		return domain_angle_value_per_edge_;
	}

	const Eigen::VectorXd& SeamlessObjective::GetDomainLengthValuePerEdge() const
	{
		return domain_length_value_per_edge_;
	}
	
	bool GetProperty(const int32_t property_id, const int32_t property_modifier_id, std::any& property_value) override
	{
		if (SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::GetProperty(property_id, property_modifier_id, property_value))
		{
			return true;
		}

		const ObjectiveFunctionBase::PropertyModifiers property_modifiers = static_cast<ObjectiveFunctionBase::PropertyModifiers>(property_modifier_id);
		const Properties properties = static_cast<Properties>(property_id);
		switch (properties)
		{
		case Properties::Zeta:
			property_value = GetZeta();
			return true;
		case Properties::AngleValuePerEdge:
			property_value = GetAngleValuePerEdge(property_modifiers);
			return true;
		case Properties::LengthValuePerEdge:
			property_value = GetLengthValuePerEdge(property_modifiers);
			return true;
		case Properties::ValuePerEdge:
			property_value = GetValuePerEdge(property_modifiers);
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

protected:
	/**
	 * Protected overrides
	 */
	void PostInitialize() override
	{
		SummationObjective<SparseObjectiveFunction<StorageOrder_>, Eigen::VectorXd>::PostInitialize();
		image_value_per_edge_.resize(this->mesh_data_provider_->GetImageEdgesCount());
		image_angle_value_per_edge_.resize(this->mesh_data_provider_->GetImageEdgesCount());
		image_length_value_per_edge_.resize(this->mesh_data_provider_->GetImageEdgesCount());
		domain_value_per_edge_.resize(this->mesh_data_provider_->GetDomainEdgesCount());
		domain_angle_value_per_edge_.resize(this->mesh_data_provider_->GetDomainEdgesCount());
		domain_length_value_per_edge_.resize(this->mesh_data_provider_->GetDomainEdgesCount());
	}

	void PostUpdate(const Eigen::VectorXd& x) override
	{
		CalculateAngleValuePerEdge(domain_angle_value_per_edge_, image_angle_value_per_edge_);
		CalculateLengthValuePerEdge(domain_length_value_per_edge_, image_length_value_per_edge_);
		CalculateValuePerEdge(domain_value_per_edge_, image_value_per_edge_);
	}
	
private:
	/**
	 * Private methods
	 */
	void CalculateValuePerEdge(Eigen::VectorXd& domain_value_per_edge, Eigen::VectorXd& image_value_per_edge)
	{
		domain_value_per_edge = domain_angle_value_per_edge_ + domain_length_value_per_edge_;
		image_value_per_edge = image_angle_value_per_edge_ + image_length_value_per_edge_;
	}

	void CalculateAngleValuePerEdge(Eigen::VectorXd& domain_angle_value_per_edge, Eigen::VectorXd& image_angle_value_per_edge)
	{
		domain_angle_value_per_edge.setZero();
		image_angle_value_per_edge.setZero();
		for(const auto& periodic_edge_pair_angle_objective : periodic_edge_pair_angle_objectives)
		{
			auto edge_pair_angle_objective = std::dynamic_pointer_cast<EdgePairAngleObjective<StorageOrder_>>(periodic_edge_pair_angle_objective->GetInnerObjective());
			const RDS::EdgeIndex domain_edge_index = edge_pair_angle_objective->GetEdgePairDataProvider().GetDomainEdgeIndex();
			const RDS::EdgeIndex image_edge_1_index = edge_pair_angle_objective->GetEdgePairDataProvider().GetImageEdge1Index();
			const RDS::EdgeIndex image_edge_2_index = edge_pair_angle_objective->GetEdgePairDataProvider().GetImageEdge2Index();

			const double value = periodic_edge_pair_angle_objective->GetValue();
			domain_angle_value_per_edge.coeffRef(domain_edge_index) += value;
			image_angle_value_per_edge.coeffRef(image_edge_1_index) += value;
			image_angle_value_per_edge.coeffRef(image_edge_2_index) += value;
		}
	}

	void CalculateLengthValuePerEdge(Eigen::VectorXd& domain_length_value_per_edge, Eigen::VectorXd& image_length_value_per_edge)
	{
		domain_length_value_per_edge.setZero();
		image_length_value_per_edge.setZero();
		for (const auto& edge_pair_length_objective : edge_pair_length_objectives)
		{
			const RDS::EdgeIndex domain_edge_index = edge_pair_length_objective->GetEdgePairDataProvider().GetDomainEdgeIndex();
			const RDS::EdgeIndex image_edge_1_index = edge_pair_length_objective->GetEdgePairDataProvider().GetImageEdge1Index();
			const RDS::EdgeIndex image_edge_2_index = edge_pair_length_objective->GetEdgePairDataProvider().GetImageEdge2Index();

			const double value = edge_pair_length_objective->GetValue();
			domain_length_value_per_edge.coeffRef(domain_edge_index) += value;
			image_length_value_per_edge.coeffRef(image_edge_1_index) += value;
			image_length_value_per_edge.coeffRef(image_edge_2_index) += value;
		}
	}
	
	/**
	 * Private fields
	 */
	double zeta_;
	std::vector<std::shared_ptr<EdgePairLengthObjective<StorageOrder_>>> edge_pair_length_objectives;
	std::vector<std::shared_ptr<EdgePairAngleObjective<StorageOrder_>>> edge_pair_angle_objectives;
	std::vector<std::shared_ptr<PeriodicObjective<StorageOrder_>>> periodic_edge_pair_angle_objectives;

	Eigen::VectorXd image_value_per_edge_;
	Eigen::VectorXd image_angle_value_per_edge_;
	Eigen::VectorXd image_length_value_per_edge_;
	Eigen::VectorXd domain_value_per_edge_;
	Eigen::VectorXd domain_angle_value_per_edge_;
	Eigen::VectorXd domain_length_value_per_edge_;
};

#endif