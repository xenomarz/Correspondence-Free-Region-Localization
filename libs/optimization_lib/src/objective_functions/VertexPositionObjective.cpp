#include <objective_functions/VertexPositionObjective.h>

VertexPositionObjective::VertexPositionObjective()
{
    name = "Positional Constraints";
	w = 10000;
}

void VertexPositionObjective::init()
{
	if(numV==0)
		throw name + " must define members numV before init()!";
	init_hessian();
}

void VertexPositionObjective::updateX(const VectorXd& X)
{
	CurrConstrainedVerticesPos.resizeLike(ConstrainedVerticesPos);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		CurrConstrainedVerticesPos.row(i) << X(ConstrainedVerticesInd[i]),X(ConstrainedVerticesInd[i] + numV);
	}
}

double VertexPositionObjective::value(bool update)
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

void VertexPositionObjective::gradient(VectorXd& g)
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
	gradient_norm = g.norm();
}

void VertexPositionObjective::hessian()
{
	fill(SS.begin(), SS.end(), 0);
	for (int i = 0; i < ConstrainedVerticesInd.size(); i++)
	{
		SS[ConstrainedVerticesInd[i]] = 2; SS[ConstrainedVerticesInd[i] + numV] = 2;
	}
}

void VertexPositionObjective::init_hessian()
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