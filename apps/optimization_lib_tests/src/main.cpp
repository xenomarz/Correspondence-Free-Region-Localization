#include "../../../libs/optimization_lib/include/utils.h"
#include "../../../libs/optimization_lib/include/solvers/NewtonSolver.h"
#include "../../../libs/optimization_lib/include/solvers/worhpSolver.h"
#include "../../../libs/optimization_lib/include/objective_functions/PenaltyPositionalConstraints.h"
#include "../../../libs/optimization_lib/include/objective_functions/LagrangianAreaStLscm.h"
#include "../../../libs/optimization_lib/include/objective_functions/TotalObjective.h"

#include <iostream>
using namespace std;

#define N 4
#define M 2


int main()
{
	//NewtonSolver solver(true,0);
	//Eigen::MatrixXd V(N / 2, 2);
	//Eigen::MatrixX3i F(M, 3);
	//Eigen::VectorXd X(N),Lambda(M);
	//V.setZero();
	//F.setZero();
	//X.setZero();
	//Lambda.setZero();

	//// initialize the energy
	//auto cF = std::make_shared<LagrangianAreaStLscm>();
	//cF->init_mesh(V, F);
	//cF->init();
	//auto cPositional = std::make_shared<PenaltyPositionalConstraints>(true);
	//cPositional->numV = V.rows();
	//cPositional->numF = F.rows();
	//cPositional->init();
	//auto totalObjective = std::make_shared<TotalObjective>();
	//totalObjective->objectiveList.clear();
	//totalObjective->init_mesh(V, F);
	//totalObjective->objectiveList.push_back(move(cF));
	//totalObjective->objectiveList.push_back(move(cPositional));
	//totalObjective->init();

	//X << -100, -100, 100, -100;
	//Lambda << 0,0;
	//
	////*worhpSolver wor;
	//wor.init(V, F);
	//wor.run(X);*/
	//
	//solver.init(totalObjective, X, Lambda, F, V);
	//solver.run();

	///*solver.init(totalObjective, X, Lambda, F, V);
	//solver.run_one_iteration(0,true);
	//solver.get_data(X, Lambda);
	//solver.init(totalObjective, X, Lambda, F, V);
	//solver.run_one_iteration(0, true);
	//solver.get_data(X, Lambda);*/

	Eigen::SparseMatrix<double> w,fsddsd;
	w.resize(2, 2);
	w.insert(0, 0) = 5;
	w.insert(0, 1) = 6;
	w.insert(1, 1) = 8;
	w.makeCompressed();
	cout << w.diagonal() << endl;
	cout << w.toDense() << endl;
	cout << w.selfadjointView<Eigen::Upper>() << endl;
	//w.triangularView<Eigen::StrictlyUpper>();
	
	fsddsd = w.selfadjointView<Eigen::Upper>();
	cout << fsddsd << endl;

	return 0;
}
