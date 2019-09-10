#include <objective_functions/penalty_positional_constraints.h>

PenaltyPositionalConstraints::PenaltyPositionalConstraints()
{
    name = "Positional Constraints";
}

void PenaltyPositionalConstraints::init()
{
	if(numV==0)
		throw "ConstraintsPositional must define members numV before init()!";
	prepare_hessian();
	w = 10000;
}

void PenaltyPositionalConstraints::updateX(const VectorXd& X)
{
	CurrConstrainedVerticesPos.resizeLike(ConstrainedVerticesPos);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		CurrConstrainedVerticesPos.row(i) << X(ConstrainedVerticesInd[i]),X(ConstrainedVerticesInd[i] + numV);
	}
}

double PenaltyPositionalConstraints::value()
{
	if (CurrConstrainedVerticesPos.rows() != ConstrainedVerticesPos.rows()) {
		return 0;
	}
	return (ConstrainedVerticesPos - CurrConstrainedVerticesPos).squaredNorm();
}

void PenaltyPositionalConstraints::gradient(VectorXd& g)
{
	g.conservativeResize(numV * 2);
	g.setZero();

	if (CurrConstrainedVerticesPos.rows() == ConstrainedVerticesPos.rows()) {
		MatrixXd diff = (CurrConstrainedVerticesPos - ConstrainedVerticesPos);
		for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
		{
			g(ConstrainedVerticesInd[i]) = 2 * diff(i, 0);
			g(ConstrainedVerticesInd[i] + numV) = 2 * diff(i, 1);
		}
	}
}

void PenaltyPositionalConstraints::hessian()
{
	fill(SS.begin(), SS.end(), 0);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		SS[ConstrainedVerticesInd[i]] = 2; SS[ConstrainedVerticesInd[i] + numV] = 2;
	}
}

void PenaltyPositionalConstraints::prepare_hessian()
{
	II.resize(2*numV);
	JJ.resize(2*numV);
	for (int i = 0; i < 2*numV; i++)
	{
		II[i] = i;
		JJ[i] = i;
	}
	SS = vector<double>(II.size(), 0.);
}