#include "objective_functions/PenaltyPositionalConstraints.h"

PenaltyPositionalConstraints::PenaltyPositionalConstraints(bool isConstrObjFunc)
{
    name = "Positional Constraints";
	w = 50;
	IsConstrObjFunc = isConstrObjFunc;
}

void PenaltyPositionalConstraints::init()
{
	if(numV==0 || numF == 0)
		throw name + " must define members numV & numF before init()!";
	init_hessian();
}

void PenaltyPositionalConstraints::updateX(const Eigen::VectorXd& X)
{
	CurrConstrainedVerticesPos.resizeLike(ConstrainedVerticesPos);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		CurrConstrainedVerticesPos.row(i) << X(ConstrainedVerticesInd[i]),X(ConstrainedVerticesInd[i] + numV);
	}
}

double PenaltyPositionalConstraints::value(const bool update)
{
	if (CurrConstrainedVerticesPos.rows() != ConstrainedVerticesPos.rows()) {
		return 0;
	}
	double E = (ConstrainedVerticesPos - CurrConstrainedVerticesPos).squaredNorm();
	if (update) {
		energy_value = E;
	}
	
	return E;
}

double PenaltyPositionalConstraints::AugmentedValue(const bool update) {
	return value(update);
}

void PenaltyPositionalConstraints::gradient(Eigen::VectorXd& g, const bool update)
{
	if (IsConstrObjFunc) {
		g.conservativeResize(numV * 2 + numF);
	}
	else {
		g.conservativeResize(numV * 2);
	}
	
	g.setZero();

	if (CurrConstrainedVerticesPos.rows() == ConstrainedVerticesPos.rows()) {
		Eigen::MatrixXd diff = (CurrConstrainedVerticesPos - ConstrainedVerticesPos);
		for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
		{
			g(ConstrainedVerticesInd[i]) = 2 * diff(i, 0);
			g(ConstrainedVerticesInd[i] + numV) = 2 * diff(i, 1);
		}
	}

	if(update)
		gradient_norm = g.norm();
}

void PenaltyPositionalConstraints::hessian()
{
	fill(SS.begin(), SS.end(), 0);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		SS[ConstrainedVerticesInd[i]] = 2; SS[ConstrainedVerticesInd[i] + numV] = 2;
	}
}

void PenaltyPositionalConstraints::init_hessian()
{
	if (IsConstrObjFunc) {
		II.resize(2 * numV + 1);
		JJ.resize(2 * numV + 1);
	}
	else {
		II.resize(2 * numV);
		JJ.resize(2 * numV);
	}
	
	for (int i = 0; i < 2*numV; i++)
	{
		II[i] = i;
		JJ[i] = i;
	}

	if (IsConstrObjFunc) {
		II[2 * numV] = 2 * numV + numF - 1;
		JJ[2 * numV] = 2 * numV + numF - 1;
	}
	SS = std::vector<double>(II.size(), 0.);
}

double PenaltyPositionalConstraints::objectiveValue(const bool update){
	double w;
	return w;
}

Eigen::VectorXd PenaltyPositionalConstraints::objectiveGradient(const bool update) {
	Eigen::VectorXd w;
	return w;
}

Eigen::SparseMatrix<double> PenaltyPositionalConstraints::objectiveHessian(std::vector<int>& I, std::vector<int>& J, std::vector<double>& S) {
	Eigen::SparseMatrix<double> w;
	return w;
}

Eigen::VectorXd PenaltyPositionalConstraints::constrainedValue(const bool update) {
	Eigen::VectorXd w;
	return w;
}

Eigen::SparseMatrix<double> PenaltyPositionalConstraints::constrainedGradient(const bool update) {
	Eigen::SparseMatrix<double> w;
	return w;
}

std::vector<Eigen::SparseMatrix<double>> PenaltyPositionalConstraints::constrainedHessian(const bool update) {
	std::vector<Eigen::SparseMatrix<double>> w;
	return w;
}

void PenaltyPositionalConstraints::lagrangianGradient(Eigen::VectorXd& g, const bool update) {

}