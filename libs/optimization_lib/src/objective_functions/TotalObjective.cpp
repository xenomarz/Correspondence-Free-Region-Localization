#include <objective_functions/TotalObjective.h>

TotalObjective::TotalObjective()
{
	name = "Total objective";
}

void TotalObjective::init()
{
	//assume that each objective's member have been set outside
	for (auto &objective : objectiveList) {
		objective->init();
	}
	init_hessian();
}

void TotalObjective::updateX(const Eigen::VectorXd& X)
{
	for (auto &objective : objectiveList)
		objective->updateX(X);
}

double TotalObjective::value(const bool update)
{
	double f=0;
	double Cconstraint_value = 0;
	double Cobjective_value = 0;
	for (auto &obj : objectiveList) {
		if (obj->w != 0) {
			std::shared_ptr<ConstrainedObjectiveFunction> constr = std::dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj);
			f += obj->w * obj->value(update);
			
			if (constr != NULL) {
				Cconstraint_value += constr->w * constr->constraint_value;
				Cobjective_value += constr->w * constr->objective_value;
			}
		}
	}
		
	if (update) {
		energy_value = f;
		constraint_value = Cconstraint_value;
		objective_value = Cobjective_value;
	}
	return f;
}

double TotalObjective::AugmentedValue(const bool update) {
	//Just for updating the data
	if (update)
		value(update);
	//augmented_value calculation...
	double f = 0;
	for (auto &objective : objectiveList)
		if (objective->w != 0)
			f += objective->w*objective->AugmentedValue(update);
	return f;
}

void TotalObjective::gradient(Eigen::VectorXd& g, const bool update)
{
	Eigen::VectorXd gi;
	g.setZero();
	for (auto &objective : objectiveList) {
		if (objective->w != 0)
		{
			objective->gradient(gi, update);
			if (g.size() == 0) {
				g.resize(gi.rows());
				g.setZero();
			}
			g += objective->w*gi;
		}
	}

	if(update){
		gradient_norm = g.norm();
		if (g.rows() == (F.rows() + 2 * V.rows())) {
			objective_gradient_norm = g.head(2 * V.rows()).norm();
			constraint_gradient_norm = g.tail(F.rows()).norm();
		}
	}
}

void TotalObjective::hessian()
{
	SS.clear();
	
	for (auto const &objective : objectiveList)
	{
        //if (objective->w != 0) //Just don't update the hessian, but we still must enter those elements into the big hessian to have the same sparsity pattern
		objective->hessian();
		std::vector<double> SSi; SSi.resize(objective->SS.size());
        for (int i = 0; i < objective->SS.size(); i++)
            SSi[i] = objective->w * objective->SS[i];

		SS.insert(SS.end(), SSi.begin(), SSi.end());
	}


	// shift the diagonal of the hessian
	int rows = *std::max_element(II.begin(), II.end()) + 1;
	std::vector<double> SSi; SSi.resize(rows);
	for (int i = 0; i < rows; i++) {
		SSi[i] = 1e-6 + Shift_eigen_values;
	}
	SS.insert(SS.end(), SSi.begin(), SSi.end());
	assert(SS.size() == II.size() && SS.size() == JJ.size());
}

void TotalObjective::init_hessian()
{
	//assume that each subobjective already prepared its hessian
	II.clear(); JJ.clear(); SS.clear();
	for (auto &objective : objectiveList)
	{
		II.insert(II.end(), objective->II.begin(), objective->II.end());
		JJ.insert(JJ.end(), objective->JJ.begin(), objective->JJ.end());
		SS.insert(SS.end(), objective->SS.begin(), objective->SS.end());
	}


	// shift the diagonal of the hessian
	int rows = *std::max_element(II.begin(), II.end()) + 1;
	std::vector<double> SSi; SSi.resize(rows);
	std::vector<double> IIi; IIi.resize(rows);
	std::vector<double> JJi; JJi.resize(rows);
	for (int i = 0; i < rows; i++) {
		IIi[i] = i;
		JJi[i] = i;
		SSi[i] = 1e-6;
	}
	SS.insert(SS.end(), SSi.begin(), SSi.end());
	II.insert(II.end(), IIi.begin(), IIi.end());
	JJ.insert(JJ.end(), JJi.begin(), JJi.end());
}