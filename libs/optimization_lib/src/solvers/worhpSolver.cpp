#include <solvers/worhpSolver.h>
#include <iostream>

#include <worhp.h>

// Declare user functions, implementation later

// Objective function
void UserF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
// Function of constraints
void UserG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
// Gradient of objective function
void UserDF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
// Jacobian of constraints
void UserDG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
// Hessian of Lagrangian
void UserHM(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);


/*-----------------------------------------------------------------------
 *
 * Minimise    f
 *
 * subject to      -0.5 <= x1 <=  INFTY
 *                   -2 <= x2 <=  INFTY
 *                    0 <= x3 <=  2
 *                   -2 <= x4 <=  2
 *                         g1 ==  1
 *               -INFTY <= g2 <= -1
 *                  2.5 <= g3 <=  5
 *
 * where         f (x1,x2,x3,x4) = x1^2 + 2 x2^2 - x3
 *               g1(x1,x2,x3,x4) = x1^2 + x3^2 + x1x3
 *               g2(x1,x2,x3,x4) = x3 - x4
 *               g3(x1,x2,x3,x4) = x2 + x4
 *
 * Optimal solution
 *                     x*  = (0, 0.5, 1, 2)
 *                   f(x*) = -0.5
 *                   g(x*) = (1, -1, 2.5)
 *
 *-----------------------------------------------------------------------*/

#include <windows.h>
using namespace std;

// The directory path returned by native GetCurrentDirectory() no end backslash
string getCurrentDirectoryOnWindows()
{
	const unsigned long maxDir = 260;
	char currentDir[maxDir];
	GetCurrentDirectory(maxDir, currentDir);
	return string(currentDir);
}

int play()
{
	auto bla = getCurrentDirectoryOnWindows();

	/*
	 * WORHP data structures
	 *
	 * OptVar contains the variables, constraint values and multipliers.
	 * Workspace encapsulates all workspace and working variables for WORHP.
	 * Params contains all WORHP parameters.
	 * Control contains things for reverse communication flow control.
	 */
	OptVar    opt;
	Workspace wsp;
	Params    par;
	Control   cnt;

	// Check Version of library and header files
	CHECK_WORHP_VERSION

		// Properly zeros everything, or else the following routines could get confused
		WorhpPreInit(&opt, &wsp, &par, &cnt);

	// Uncomment this to get more info on data structures
	//WorhpDiag(&opt, &wsp, &par, &cnt);

	/*
	 * Parameter initialisation routine that must be called
	 * when using ReadParamsNoInit instead of ReadParams.
	 */
	int status;
	InitParams(&status, &par);

	/*
	 * We can now set parameters that may be overruled by those in the
	 * parameter file. This is useful for setting a non-default standard
	 * parameter value that may still be overwritten.
	 */
	par.NLPprint = 1;  // Let's prefer the slim output format
					   // unless the parameter file says differently

	/*
	 * Parameter XML import routine that does not reset
	 * all parameters to default values (InitParams does this)
	 */
	ReadParamsNoInit(&status, "worhp.xml", &par);
	if (status == DataError || status == InitError)
	{
		return EXIT_FAILURE;
	}

	/*
	 * WORHP data structure initialisation routine.
	 * Calling this routine prior to WORHP is mandatory.
	 * Before calling WorhpInit, set the problem and matrix dimensions as
	 *
	 * opt.n      = number of variables,
	 * opt.m      = number of constraints (lin + nonlin, excluding box con's),
	 * wsp.DF.nnz = nonzero entries of the objective function gradient,
	 * wsp.DG.nnz = nonzero entries of the constraint Jacobian,
	 * wsp.HM.nnz = nonzero entries of the Lagrange Hessian.
	 *
	 * Set nnz to 'WorhpMatrix_Init_Dense' to have WorhpInit allocate and
	 * create a dense matrix structure appropriate for the matrix kind and
	 * its dimensions. Setting it to its dense dimension achieves the same.
	 */
	opt.n = 4;  // This problem has 4 variables
	opt.m = 3;  // and 3 constraints (excluding box constraints)

	// All derivatives for this problem have a sparse structure, so
	// set the amount of nonzeros here
	wsp.DF.nnz = 3;
	wsp.DG.nnz = 6;
	wsp.HM.nnz = 1 + opt.n;  // 1 entry on strict lower triangle
							 // plus full diagonal

	WorhpInit(&opt, &wsp, &par, &cnt);
	if (cnt.status != FirstCall)
	{
		std::cout << "Main: Initialisation failed." << std::endl;
		return EXIT_FAILURE;
	}

	/*
	 * Set function types (optional). Possible values are:
	 * WORHP_CONSTANT, WORHP_LINEAR, WORHP_QUADRATIC, WORHP_NONLINEAR
	 */
	opt.FType = WORHP_QUADRATIC;
	opt.GType[0] = WORHP_QUADRATIC;
	opt.GType[1] = WORHP_LINEAR;
	opt.GType[2] = WORHP_LINEAR;

	/*
	 * These pointers give access to the essential user data:
	 *
	 * opt.X[0] to opt.X[opt.n - 1]           : Optimisation variables
	 * opt.Lambda[0] to opt.Lambda[opt.n - 1] : Multipliers for the constraints
	 *                                          on X ("box constraints")
	 * opt.G[0] to opt.G[opt.m - 1]           : Linear and nonlinear constraints
	 * opt.Mu[0] to opt.Mu[opt.m - 1]         : Multipliers for the constraints on G
	 *
	 * Set initial values of X, Lambda and Mu here.
	 * G need not be initialised.
	 */
	opt.X[0] = 2.0;
	opt.X[1] = 2.0;
	opt.X[2] = 1.0;
	opt.X[3] = 0.0;
	opt.Lambda[0] = 0.0;
	opt.Lambda[1] = 0.0;
	opt.Lambda[2] = 0.0;
	opt.Lambda[3] = 0.0;
	opt.Mu[0] = 0.0;
	opt.Mu[1] = 0.0;
	opt.Mu[2] = 0.0;

	/*
	 * Set lower and upper bounds on the variables and constraints.
	 * Use +/-par.Infty to signal "unbounded".
	 *
	 * XL and XU are lower and upper bounds ("box constraints") on X.
	 * GL and GU are lower and upper bounds on G.
	 */
	opt.XL[0] = -0.5;
	opt.XU[0] = par.Infty;
	opt.XL[1] = -2.0;
	opt.XU[1] = par.Infty;
	opt.XL[2] = 0.0;
	opt.XU[2] = 2.0;
	opt.XL[3] = -2.0;
	opt.XU[3] = 2.0;

	opt.GL[0] = 1.0;  // set opt.GL[i] == opt.GU[i]
	opt.GU[0] = 1.0;  // for equality constraints
	opt.GL[1] = -par.Infty;
	opt.GU[1] = -1.0;
	opt.GL[2] = 2.5;
	opt.GU[2] = 5.0;

	/*
	 * Specify matrix structures in CS format, using Fortran indexing,
	 * i.e. 1...N instead of 0...N-1, to describe the matrix structure.
	 */

	 // Define DF as row vector, column index is ommited
	if (wsp.DF.NeedStructure)
	{
		// only set the nonzero entries, so omit the 4th entry,
		// which is a structural zero
		wsp.DF.row[0] = 1;
		wsp.DF.row[1] = 2;
		wsp.DF.row[2] = 3;
	}

	// Define DG as CS-matrix
	if (wsp.DG.NeedStructure)
	{
		// only set the nonzero entries in column-major order
		wsp.DG.row[0] = 1;
		wsp.DG.col[0] = 1;

		wsp.DG.row[1] = 3;
		wsp.DG.col[1] = 2;

		wsp.DG.row[2] = 1;
		wsp.DG.col[2] = 3;

		wsp.DG.row[3] = 2;
		wsp.DG.col[3] = 3;

		wsp.DG.row[4] = 2;
		wsp.DG.col[4] = 4;

		wsp.DG.row[5] = 3;
		wsp.DG.col[5] = 4;
	}

	// Define HM as a diagonal LT-CS-matrix, but only if needed by WORHP
	if (wsp.HM.NeedStructure)
	{
		/*
		 * only set the nonzero entries, with the strictly lower triangle first,
		 * followed by ALL diagonal entries, so even the entry at (4, 4)
		 * even though it is a structural zero
		 */

		 // strict lower triangle
		wsp.HM.row[0] = 3;
		wsp.HM.col[0] = 1;

		// diagonal
		for (int i = 0; i < opt.n; i += 1)
		{
			wsp.HM.row[wsp.HM.nnz - opt.n + i] = i + 1;
			wsp.HM.col[wsp.HM.nnz - opt.n + i] = i + 1;
		}
	}

	/*
	 * WORHP Reverse Communication loop.
	 * In every iteration poll GetUserAction for the requested action, i.e. one
	 * of {callWorhp, iterOutput, evalF, evalG, evalDF, evalDG, evalHM, fidif}.
	 *
	 * Make sure to reset the requested user action afterwards by calling
	 * DoneUserAction, except for 'callWorhp' and 'fidif'.
	 */
	while (cnt.status < TerminateSuccess && cnt.status > TerminateError)
	{
		/*
		 * WORHP's main routine.
		 * Do not manually reset callWorhp, this is only done by the FD routines.
		 */
		if (GetUserAction(&cnt, callWorhp))
		{
			Worhp(&opt, &wsp, &par, &cnt);
			// No DoneUserAction!
		}

		/*
		 * Show iteration output.
		 * The call to IterationOutput() may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, iterOutput))
		{
			IterationOutput(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, iterOutput);
		}

		/*
		 * Evaluate the objective function.
		 * The call to UserF may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, evalF))
		{
			UserF(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, evalF);
		}

		/*
		 * Evaluate the constraints.
		 * The call to UserG may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, evalG))
		{
			UserG(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, evalG);
		}

		/*
		 * Evaluate the gradient of the objective function.
		 * The call to UserDF may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, evalDF))
		{
			UserDF(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, evalDF);
		}

		/*
		 * Evaluate the Jacobian of the constraints.
		 * The call to UserDG may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, evalDG))
		{
			UserDG(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, evalDG);
		}

		/*
		 * Evaluate the Hessian matrix of the Lagrange function (L = f + mu*g)
		 * The call to UserHM may be replaced by user-defined code.
		 */
		if (GetUserAction(&cnt, evalHM))
		{
			UserHM(&opt, &wsp, &par, &cnt);
			DoneUserAction(&cnt, evalHM);
		}

		/*
		 * Use finite differences with RC to determine derivatives
		 * Do not reset fidif, this is done by the FD routine.
		 */
		if (GetUserAction(&cnt, fidif))
		{
			WorhpFidif(&opt, &wsp, &par, &cnt);
			// No DoneUserAction!
		}
	}

	// Translate the WORHP status flag into a meaningful message.
	StatusMsg(&opt, &wsp, &par, &cnt);
	// Deallocate all data structures.
	// Data structures must not be accessed after this call.
	WorhpFree(&opt, &wsp, &par, &cnt);

	return EXIT_SUCCESS;
}


void UserF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	double* X = opt->X;  // Abbreviate notation

	opt->F = wsp->ScaleObj * (X[0] * X[0] + 2.0 * X[1] * X[1] - X[2]);
}

void UserG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	double* X = opt->X;  // Abbreviate notation

	opt->G[0] = X[0] * X[0] + X[2] * X[2] + X[0] * X[2];
	opt->G[1] = X[2] - X[3];
	opt->G[2] = X[1] + X[3];
}

void UserDF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	wsp->DF.val[0] = wsp->ScaleObj * 2.0 * opt->X[0];
	wsp->DF.val[1] = wsp->ScaleObj * 4.0 * opt->X[1];
	wsp->DF.val[2] = wsp->ScaleObj * -1.0;
}

void UserDG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	wsp->DG.val[0] = 2.0 * opt->X[0] + opt->X[2];
	wsp->DG.val[1] = 1.0;
	wsp->DG.val[2] = opt->X[0] + 2.0 * opt->X[2];
	wsp->DG.val[3] = 1.0;
	wsp->DG.val[4] = -1.0;
	wsp->DG.val[5] = 1.0;
}

void UserHM(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	// Only scale the F part of HM
	wsp->HM.val[0] = opt->Mu[0];
	wsp->HM.val[1] = wsp->ScaleObj * 2.0 + 2.0 * opt->Mu[0];
	wsp->HM.val[2] = wsp->ScaleObj * 4.0;
	wsp->HM.val[3] = 2.0 * opt->Mu[0];
	wsp->HM.val[4] = 0.0;
}


#define SAVE_RESULTS_TO_CSV

worhpSolver::worhpSolver(const bool isConstrObjFunc, const int solverID)
	:
	solverID(solverID),
	parameters_mutex(make_unique<mutex>()),
	data_mutex(make_unique<shared_timed_mutex>()),
	param_cv(make_unique<condition_variable>()),
	num_steps(2147483647),
	IsConstrObjFunc(isConstrObjFunc)
{
#ifdef SAVE_RESULTS_TO_CSV
	//save data in csv files
	string path = "C:\\Users\\user\\Desktop\\Solver" + std::to_string(solverID) + "\\";
	mkdir(path.c_str());
	SearchDirInfo.open(path + "SearchDirInfo.csv");
	solverInfo.open(path + "solverInfo.csv");
	hessianInfo.open(path + "hessianInfo.csv");
#endif
}

worhpSolver::~worhpSolver() {
#ifdef SAVE_RESULTS_TO_CSV
	//close csv files
	SearchDirInfo.close();
	solverInfo.close();
	hessianInfo.close();
	cout << ">> csv files has been closed!" << endl;
#endif
}

void worhpSolver::init(shared_ptr<ObjectiveFunction> objective, const VectorXd& X0)
{
	this->objective = objective;
	if (IsConstrObjFunc) { //for constraint objective function
		X.resize(X0.rows() + F.rows());
		X.head(X0.rows()) = X0;
		X.tail(F.rows()) = VectorXd::Zero(F.rows());
		ext_x = X.head(X.rows() - F.rows());
	}
	else { //for unconstraint objective function
		X = X0;
		ext_x = X;
	}
	internal_init();
}

void worhpSolver::setFlipAvoidingLineSearch(MatrixX3i & F)
{
	FlipAvoidingLineSearch = true;
	this->F = F;
}

int worhpSolver::run()
{
	cout << ">> worhp solver!" << endl;

	play();

	/*is_running = true;
	halt = false;
	int steps = 0;
	do {
		run_one_iteration(steps);
	} while ((a_parameter_was_updated || test_progress()) && !halt && ++steps < num_steps);
	is_running = false;
	cout << ">> solver stopped" << endl;*/
	return 0;
}

void worhpSolver::run_one_iteration(const int steps) {
	currentEnergy = step();
#ifdef SAVE_RESULTS_TO_CSV
	saveSolverInfo(steps, solverInfo);
	saveHessianInfo(steps, hessianInfo);
	saveSearchDirInfo(steps, SearchDirInfo);
#endif  
	if (lineSearch_type == Utils::GradientNorm)
		gradNorm_linesearch(SearchDirInfo);
	else
		value_linesearch(SearchDirInfo);
	update_external_data();
}

void worhpSolver::saveSolverInfo(int numIteration, std::ofstream& solverInfo) {
	//show only once the objective's function data
	shared_ptr<TotalObjective> totalObj = dynamic_pointer_cast<TotalObjective>(objective);
	if (!numIteration) {
		if(IsConstrObjFunc)
			solverInfo << "Obj name,weight,Augmented parameter," << endl;
		else 
			solverInfo << "Obj name,weight," << endl;
		for (auto& obj : totalObj->objectiveList) {
			solverInfo << obj->name << "," << obj->w << ",";
			if(IsConstrObjFunc)
				solverInfo << dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << "," ;
			solverInfo << endl;
		}
		solverInfo << endl << endl;

		if (IsConstrObjFunc)
			solverInfo << ",," << totalObj->name << ",,,,,,,";
		else 
			solverInfo << ",," << totalObj->name << ",,,";
		for (auto& obj : totalObj->objectiveList) {
			if (IsConstrObjFunc)
				solverInfo << obj->name << ",,,,,,,";
			else 
				solverInfo << obj->name << ",,,";
		}
		solverInfo << endl;
		if (IsConstrObjFunc)
			solverInfo << "Round,,value,obj value,constr value,grad,obj grad,constr grad,";
		else 
			solverInfo << "Round,,value,grad,";
		for (auto& obj : totalObj->objectiveList) {
			if (IsConstrObjFunc)
				solverInfo << ",value,obj value,constr value,grad,obj grad,constr grad,";
			else
				solverInfo << ",value,grad,";
		}
		solverInfo << endl;
	}

	if (IsConstrObjFunc)
		solverInfo <<
			numIteration << ",," <<
			totalObj->energy_value << "," <<
			totalObj->objective_value << "," <<
			totalObj->constraint_value << "," <<
			totalObj->gradient_norm << "," <<
			totalObj->objective_gradient_norm << "," <<
			totalObj->constraint_gradient_norm << ",,";
	else
		solverInfo <<
		numIteration << ",," <<
		totalObj->energy_value << "," <<
		totalObj->gradient_norm << ",,";

	for (auto& obj : totalObj->objectiveList) {
		shared_ptr<ConstrainedObjectiveFunction> constr = dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj);
		if (IsConstrObjFunc)
			solverInfo <<
				constr->energy_value << "," <<
				constr->objective_value << ","<<
				constr->constraint_value << "," <<
				constr->gradient_norm << "," <<
				constr->objective_gradient_norm << "," <<
				constr->constraint_gradient_norm << ",,";
		else
			solverInfo <<
				obj->energy_value << "," <<
				obj->gradient_norm << ",,";
	}
	solverInfo << endl;
}

void worhpSolver::saveHessianInfo(int numIteration, std::ofstream& hessianInfo) {
	//show only once the objective's function data
	if (!numIteration) {
		shared_ptr<TotalObjective> t = dynamic_pointer_cast<TotalObjective>(objective);
		hessianInfo << "Obj name,weight,";
		if(IsConstrObjFunc)
			hessianInfo << "Augmented parameter,";
		hessianInfo << endl;
		for (auto& obj : t->objectiveList) {
			hessianInfo << obj->name << "," << obj->w << ",";
			if (IsConstrObjFunc)
				hessianInfo << dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << ",";
			hessianInfo << endl;
		}
		hessianInfo << endl;
	}

	
	//prepare the hessian matrix
	Eigen::SparseMatrix<double> A;
	std::vector<Eigen::Triplet<double>> tripletList;
	tripletList.reserve((objective->II).size());
	int rows = *std::max_element((objective->II).begin(), (objective->II).end()) + 1;
	int cols = *std::max_element((objective->JJ).begin(), (objective->JJ).end()) + 1;
	assert(rows == cols && "Rows == Cols at solver's saveHessianInfo() method");
	for (int i = 0; i < (objective->II).size(); i++)
		tripletList.push_back(Eigen::Triplet<double>((objective->II)[i], (objective->JJ)[i], (objective->SS)[i]));
	A.resize(rows, cols);
	A.setFromTriplets(tripletList.begin(), tripletList.end());

	//output the hessian
	hessianInfo << ("Round " + std::to_string(numIteration)).c_str() << endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			hessianInfo << A.coeff(i, j) << ",";
		}
		
		hessianInfo << "," << g(i) << endl;
	}
	hessianInfo << endl;
}

void worhpSolver::saveSearchDirInfo(int numIteration, std::ofstream& SearchDirInfo) {
	//calculate values in the search direction vector
	int counter;
	double alpha = 0;
	for ( alpha = -1, counter = 0; alpha <= 1; alpha += 0.005, counter++) {
		MatrixXd curr_x = X + alpha * p;
		VectorXd grad;
		objective->updateX(curr_x);
		objective->gradient(grad,false);
		alfa[counter] = alpha;
		y_value[counter] = objective->value(false);
		y_augmentedValue[counter] = objective->AugmentedValue(false);
		y_gradientNorm[counter] = grad.norm();
	}
	cout << "counter = " << counter << endl;
	objective->updateX(X);

	//show only once the objective's function data
	if (!numIteration) {
		shared_ptr<TotalObjective> t = dynamic_pointer_cast<TotalObjective>(objective);
		SearchDirInfo << "Obj name,weight,";
		if (IsConstrObjFunc)
			SearchDirInfo << "Augmented parameter,";
		SearchDirInfo << endl;
		for (auto& obj : t->objectiveList) {
			SearchDirInfo << obj->name << "," << obj->w << ",";
			if (IsConstrObjFunc)
				SearchDirInfo << dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << ",";
			SearchDirInfo << endl;
		}
		SearchDirInfo << endl << "Round" << endl;
	}
		
	//add the alfa values as one row
	SearchDirInfo << numIteration << ",";
	SearchDirInfo << "alfa,";
	for (int i = 0; i < counter; i++) {
		SearchDirInfo << alfa[i] << ",";
	}
	SearchDirInfo << endl;

	//add the total objective's values as one row
	SearchDirInfo << ",value,";
	for (int i = 0; i < counter; i++) {
		SearchDirInfo << y_value[i] << ",";
	}
	SearchDirInfo << endl;

	//add the total objective's augmented values as one row
	if (IsConstrObjFunc) {
		SearchDirInfo << ",augmentedValue,";
		for (int i = 0; i < counter; i++) {
			SearchDirInfo << y_augmentedValue[i] << ",";
		}
		SearchDirInfo << endl;
	}
	//add the total objective's augmented values as one row
	if (IsConstrObjFunc) {
		SearchDirInfo << ",grad norm,";
		for (int i = 0; i < counter; i++) {
			SearchDirInfo << y_gradientNorm[i] << ",";
		}
		SearchDirInfo << endl;
	}
}

void worhpSolver::value_linesearch(std::ofstream& SearchDirInfo)
{
	double step_size;
	if (/*FlipAvoidingLineSearch*/false)
	{
		double min_step_to_singularity;
		if (IsConstrObjFunc) {
			auto MatX = Map<MatrixX2d>(X.head(X.rows() - F.rows()).data(), X.head(X.rows() - F.rows()).rows() / 2, 2);
			MatrixXd MatP = Map<const MatrixX2d>(p.head(X.rows() - F.rows()).data(), p.head(X.rows() - F.rows()).rows() / 2, 2);
			min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(MatX, F, MatP);
		}
		else {
			auto MatX = Map<MatrixX2d>(X.data(), X.rows() / 2, 2);
			MatrixXd MatP = Map<const MatrixX2d>(p.data(), p.rows() / 2, 2);
			min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(MatX, F, MatP);
		}
		step_size = min(1., min_step_to_singularity*0.8);
	}
	else
		step_size = 1;

	double new_energy = currentEnergy;
	
	int cur_iter = 0; int MAX_STEP_SIZE_ITER = 50;

	while (cur_iter < MAX_STEP_SIZE_ITER)
	{
		MatrixXd curr_x = X + step_size * p;

		objective->updateX(curr_x);

		if (IsConstrObjFunc) 
			new_energy = objective->AugmentedValue(false);
		else
			new_energy = objective->value(false);
		
		if (new_energy >= currentEnergy)
		{
			step_size /= 2;
		}
		else
		{
			X = curr_x;
			break;
		}
		cur_iter++;
	}

	if (cur_iter == MAX_STEP_SIZE_ITER) {
		cur_iter = 0;
		step_size = -1;
		while (cur_iter < MAX_STEP_SIZE_ITER)
		{
			MatrixXd curr_x = X + step_size * p;

			objective->updateX(curr_x);

			if (IsConstrObjFunc)
				new_energy = objective->AugmentedValue(false);
			else
				new_energy = objective->value(false);

			if (new_energy >= currentEnergy)
			{
				step_size /= 2;
			}
			else
			{
				X = curr_x;
				break;
			}
			cur_iter++;
		}
	}

#ifdef SAVE_RESULTS_TO_CSV
	//add the solver's choice of alfa
	if (lineSearch_type == Utils::GradientNorm)
		SearchDirInfo << ",line search type,Gradient norm," << endl;
	else
		SearchDirInfo << ",line search type,Function value," << endl;
	SearchDirInfo << ",Chosen alfa," << step_size << "," << endl;
	SearchDirInfo << ",LineSearch iter," << cur_iter << "," << endl;
#endif
}

void worhpSolver::gradNorm_linesearch(std::ofstream& SearchDirInfo)
{
	double step_size = 1;
	VectorXd grad;

	objective->updateX(X);
	objective->gradient(grad,false);
	double current_GradNrom = grad.norm();
	double new_GradNrom = current_GradNrom;

	int cur_iter = 0; int MAX_STEP_SIZE_ITER = 50;

	while (cur_iter < MAX_STEP_SIZE_ITER)
	{
		MatrixXd curr_x = X + step_size * p;

		objective->updateX(curr_x);
		objective->gradient(grad,false);
		new_GradNrom = grad.norm();
		
		if (new_GradNrom >= current_GradNrom)
		{
			step_size /= 2;
		}
		else
		{
			X = curr_x;
			break;
		}
		cur_iter++;
	}

	if (cur_iter == MAX_STEP_SIZE_ITER) {
		cur_iter = 0;
		step_size = -1;
	
		while (cur_iter < MAX_STEP_SIZE_ITER)
		{
			MatrixXd curr_x = X + step_size * p;

			objective->updateX(curr_x);
			objective->gradient(grad, false);
			new_GradNrom = grad.norm();

			if (new_GradNrom >= current_GradNrom)
			{
				step_size /= 2;
			}
			else
			{
				X = curr_x;
				break;
			}
			cur_iter++;
		}
	
	}

#ifdef SAVE_RESULTS_TO_CSV
	//add the solver's choice of alfa
	if(lineSearch_type == Utils::GradientNorm)
		SearchDirInfo << ",line search type,Gradient norm," << endl;
	else
		SearchDirInfo << ",line search type,Function value," << endl;
	SearchDirInfo << ",Chosen alfa," << step_size << "," << endl;
	SearchDirInfo << ",LineSearch iter," << cur_iter << "," << endl;
#endif
}

void worhpSolver::stop()
{
	wait_for_parameter_update_slot();
	halt = true;
	release_parameter_update_slot();
}

void worhpSolver::update_external_data()
{
	give_parameter_update_slot();
	unique_lock<shared_timed_mutex> lock(*data_mutex);
	if(IsConstrObjFunc)
		ext_x = X.head(X.rows() - F.rows());
	else 
		ext_x = X;
	progressed = true;
}

void worhpSolver::get_data(VectorXd& X)
{
	unique_lock<shared_timed_mutex> lock(*data_mutex);
	X = ext_x;
	progressed = false;
}

void worhpSolver::give_parameter_update_slot()
{
	a_parameter_was_updated = false;
	unique_lock<mutex> lock(*parameters_mutex);
	params_ready_to_update = true;
	param_cv->notify_one();
	while (wait_for_param_update)
	{
		param_cv->wait(lock);
		a_parameter_was_updated = true;
	}
	params_ready_to_update = false;
}

void worhpSolver::wait_for_parameter_update_slot()
{
	unique_lock<mutex> lock(*parameters_mutex);
	wait_for_param_update = true;
	while (!params_ready_to_update && is_running)
		param_cv->wait_for(lock, chrono::milliseconds(50));
}

void worhpSolver::release_parameter_update_slot()
{
	wait_for_param_update = false;
	param_cv->notify_one();
}

