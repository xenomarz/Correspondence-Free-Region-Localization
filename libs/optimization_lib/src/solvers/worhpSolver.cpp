#include <solvers/worhpSolver.h>


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
worhpSolver::worhpSolver() {
	this->functionF = new LagrangianLscmStArea();
	this->functionG = new LagrangianLscmStArea();
	IsDataReady = false;
}

worhpSolver::~worhpSolver() {
	delete this->functionF;
	delete this->functionG;
}

VectorXd worhpSolver::run(
	const MatrixXd& V,
	const MatrixX3i& F,
	const VectorXd& initialPoint) 
{
	IsDataReady = false;
	this->functionF->init_mesh(V, F);
	this->functionG->init_mesh(V, F);
	this->functionF->init();
	this->functionG->init();
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
	ReadParamsNoInit(&status, "worhpFD.xml", &par);
	if (status == DataError || status == InitError)
	{
		exit(EXIT_FAILURE);
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
	opt.n = V.rows() * 2;  // This problem has 4 variables
	opt.m = F.rows();  // and 3 constraints (excluding box constraints)

	// All derivatives for this problem have a sparse structure, so
	// set the amount of nonzeros here
	//wsp.DF.nnz = opt.n;
	//wsp.DG.nnz = opt.n * opt.m;
	//wsp.HM.nnz = 1 + opt.n;  // 1 entry on strict lower triangle
							 // plus full diagonal

	WorhpInit(&opt, &wsp, &par, &cnt);
	if (cnt.status != FirstCall)
	{
		std::cout << "Main: Initialisation failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	/*
	 * Set function types (optional). Possible values are:
	 * WORHP_CONSTANT, WORHP_LINEAR, WORHP_QUADRATIC, WORHP_NONLINEAR
	 */
	/*opt.FType = WORHP_QUADRATIC;
	opt.GType[0] = WORHP_QUADRATIC;
	opt.GType[1] = WORHP_LINEAR;
	opt.GType[2] = WORHP_LINEAR;*/

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

	if ((V.rows() * 2) != initialPoint.rows()) {
		assert("Error");
	}

	for (int i = 0; i < opt.n; i++) {
		opt.X[i] = initialPoint(i);
	}
	
	for (int i = 0; i < opt.n; i++) {
		opt.Lambda[i] = 0.0;
	}
	
	for (int i = 0; i < opt.m; i++) {
		opt.Mu[i] = 0.0;
	}

	/*
	 * Set lower and upper bounds on the variables and constraints.
	 * Use +/-par.Infty to signal "unbounded".
	 *
	 * XL and XU are lower and upper bounds ("box constraints") on X.
	 * GL and GU are lower and upper bounds on G.
	 */

	for (int i = 0; i < opt.n; i++) {
		opt.XL[i] = -par.Infty;
		opt.XU[i] = par.Infty;
	}

	for (int i = 0; i < opt.m; i++) {
		opt.GL[i] = 0.0;  // set opt.GL[i] == opt.GU[i]
		opt.GU[i] = 0.0;  // for equality constraints
	}


	/*
	 * Specify matrix structures in CS format, using Fortran indexing,
	 * i.e. 1...N instead of 0...N-1, to describe the matrix structure.
	 */

	 // Define DF as row vector, column index is ommited
	if (wsp.DF.NeedStructure)
	{
		cout << "------------wsp.DF.NeedStructure" << endl;

		// only set the nonzero entries, so omit the 4th entry,
		// which is a structural zero
		wsp.DF.row[0] = 1;
		wsp.DF.row[1] = 2;
		wsp.DF.row[2] = 3;
	}

	// Define DG as CS-matrix
	if (wsp.DG.NeedStructure)
	{
		cout << "------------wsp.DG.NeedStructure" << endl;

		// only set the nonzero entries in column-major order
		wsp.DG.row[0] = 1;
		wsp.DG.col[0] = 1;

		wsp.DG.row[1] = 3; // num of constr func
		wsp.DG.col[1] = 2; // num of variable

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
		cout << "------------wsp.HM.NeedStructure" << endl;
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
			update_data(&opt);

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
	return lastX;
}

void worhpSolver::update_data(OptVar* opt) {
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	IsDataReady = true;
	lastX = Map<VectorXd>(opt->X, opt->n);
}

int worhpSolver::get_data(VectorXd& data) {
	if (IsDataReady) {
		IsDataReady = false;
		data = lastX;
		return 1;
	}
	else return 0;
}

void worhpSolver::UserF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	VectorXd X = Map<VectorXd>(opt->X, opt->n);
	LagrangianLscmStArea f = *this->functionF;
	f.updateX(X);

	VectorXd LSCM = (f.a - f.d).cwiseAbs2() + (f.b + f.c).cwiseAbs2();
	//VectorXd AREA = f.detJ - VectorXd::Ones(opt->m);

	double obj_value = (f.Area.asDiagonal() * LSCM).sum();
	opt->F = wsp->ScaleObj * obj_value;
}

void worhpSolver::UserG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	VectorXd X = Map<VectorXd>(opt->X, opt->n);
	LagrangianLscmStArea f = *this->functionG;
	f.updateX(X);
	
	
	for (int i = 0; i < opt->m; i++) {
		double AREA = (f.detJ(i) - 1);
		opt->G[i] = f.Area(i) * AREA;
	}
}

void worhpSolver::UserDF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	cout << "-------------UserDF" << endl;
	wsp->DF.val[0] = wsp->ScaleObj * 2.0 * opt->X[0];
	wsp->DF.val[1] = wsp->ScaleObj * 4.0 * opt->X[1];
	wsp->DF.val[2] = wsp->ScaleObj * -1.0;
}

void worhpSolver::UserDG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	cout << "-------------UserDG" << endl;
	wsp->DG.val[0] = 2.0 * opt->X[0] + opt->X[2];
	wsp->DG.val[1] = 1.0;
	wsp->DG.val[2] = opt->X[0] + 2.0 * opt->X[2];
	wsp->DG.val[3] = 1.0;
	wsp->DG.val[4] = -1.0;
	wsp->DG.val[5] = 1.0;
}

void worhpSolver::UserHM(OptVar* opt, Workspace* wsp, Params* par, Control* cnt)
{
	cout << "-------------UserHM" << endl;
	// Only scale the F part of HM
	wsp->HM.val[0] = opt->Mu[0];
	wsp->HM.val[1] = wsp->ScaleObj * 2.0 + 2.0 * opt->Mu[0];
	wsp->HM.val[2] = wsp->ScaleObj * 4.0;
	wsp->HM.val[3] = 2.0 * opt->Mu[0];
	wsp->HM.val[4] = 0.0;
}

