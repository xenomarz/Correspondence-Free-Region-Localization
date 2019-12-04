#include <solvers/solver.h>

solver::solver(const bool isConstrObjFunc, const int solverID)
	:
	solverID(solverID),
	parameters_mutex(make_unique<mutex>()),
	data_mutex(make_unique<shared_timed_mutex>()),
	param_cv(make_unique<condition_variable>()),
	num_steps(2147483647),
	IsConstrObjFunc(isConstrObjFunc)
{
	//save data in csv files
	string path = "C:\\Users\\user\\Desktop\\Solver" + std::to_string(solverID) + "\\";
	mkdir(path.c_str());
	SearchDirInfo.open(path + "SearchDirInfo.csv");
	solverInfo.open(path + "solverInfo.csv");
	hessianInfo.open(path + "hessianInfo.csv");
}

solver::~solver() {
	//close csv files
	SearchDirInfo.close();
	solverInfo.close();
	hessianInfo.close();
	cout << "csv files has been closed!";
}

void solver::init(shared_ptr<ObjectiveFunction> objective, const VectorXd& X0)
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

void solver::setFlipAvoidingLineSearch(MatrixX3i & F)
{
	FlipAvoidingLineSearch = true;
	this->F = F;
}

int solver::run()
{
	is_running = true;
	halt = false;
	int steps = 0;
	do
	{
		currentEnergy = step();
		saveSearchDirInfo(steps, SearchDirInfo);
		saveSolverInfo(steps, solverInfo);
		saveHessianInfo(steps, hessianInfo);
		linesearch(SearchDirInfo);
		update_external_data();
	} while ((a_parameter_was_updated || test_progress()) && !halt && ++steps < num_steps);
	is_running = false;
	cout << ">> solver stopped" << endl;
	return 0;
}

void solver::saveSolverInfo(int numIteration, std::ofstream& solverInfo) {
	//show only once the objective's function data
	shared_ptr<TotalObjective> totalObj = dynamic_pointer_cast<TotalObjective>(objective);
	if (!numIteration) {
		solverInfo << "Obj name,weight,Augmented parameter," << endl;
		for (auto& obj : totalObj->objectiveList) {
			solverInfo << obj->name << "," << obj->w << "," << obj->augmented_value_parameter << "," << endl;
		}
		solverInfo << endl << endl;

		solverInfo << ",," << totalObj->name << ",,,,,,,";
		for (auto& obj : totalObj->objectiveList) {
			solverInfo << obj->name << ",,,,,,,";
		}

		solverInfo << endl
			<< "Round,,value,obj value,constr value,grad,obj grad,constr grad,";
		for (auto& obj : totalObj->objectiveList) {
			solverInfo << ",value,obj value,constr value,grad,obj grad,constr grad,";
		}
		solverInfo << endl;
	}

	solverInfo <<
		numIteration << ",," <<
		totalObj->energy_value << "," <<
		totalObj->objective_value << "," <<
		totalObj->constraint_value << "," <<
		totalObj->gradient_norm << "," <<
		totalObj->objective_gradient_norm << "," <<
		totalObj->constraint_gradient_norm << ",,";
	for (auto& obj : totalObj->objectiveList) {
		solverInfo <<
			obj->energy_value << "," << 
			obj->objective_value << ","<< 
			obj->constraint_value << "," <<
			obj->gradient_norm << "," << 
			obj->objective_gradient_norm << "," <<
			obj->constraint_gradient_norm << ",,";
	}
	solverInfo << endl;
}

void solver::saveHessianInfo(int numIteration, std::ofstream& hessianInfo) {
	//show only once the objective's function data
	if (!numIteration) {
		shared_ptr<TotalObjective> a = dynamic_pointer_cast<TotalObjective>(objective);
		hessianInfo << "Obj name,weight,Augmented parameter," << endl;
		for (auto& obj : a->objectiveList) {
			hessianInfo << obj->name << "," << obj->w << "," << obj->augmented_value_parameter << "," << endl;
		}
		hessianInfo << endl;
	}

	//prepare the hessian
	objective->updateX(X);
	objective->hessian();
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
	hessianInfo << A << endl << endl;
}

void solver::saveSearchDirInfo(int numIteration, std::ofstream& SearchDirInfo) {
	//calculate values in the search direction vector
	int counter;
	double alpha = 0;
	for ( alpha = -3, counter = 0; alpha <= 3; alpha += 0.01, counter++) {
		MatrixXd curr_x = X + alpha * p;
		objective->updateX(curr_x);
		alfa[counter] = alpha;
		y_value[counter] = objective->value();
		y_augmentedValue[counter] = objective->AugmentedValue();
	}
	objective->updateX(X);

	//show only once the objective's function data
	if (!numIteration) {
		shared_ptr<TotalObjective> a =  dynamic_pointer_cast<TotalObjective>(objective);
		SearchDirInfo << "Obj name,weight,Augmented parameter," << endl;
		for (auto& obj : a->objectiveList) {
			SearchDirInfo << obj->name << "," << obj->w << "," << obj->augmented_value_parameter << "," << endl;
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
}

void solver::linesearch(std::ofstream& SearchDirInfo)
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
	
	int cur_iter = 0; int MAX_STEP_SIZE_ITER = 12;

	while (cur_iter < MAX_STEP_SIZE_ITER)
	{
		MatrixXd curr_x = X + step_size * p;

		objective->updateX(curr_x);

		if (IsConstrObjFunc) 
			new_energy = objective->AugmentedValue();
		else
			new_energy = objective->value();
		
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

	//add the solver's choice of alfa
	SearchDirInfo << ",Chosen alfa," << step_size << "," << endl;
	SearchDirInfo << ",LineSearch iter," << cur_iter << "," << endl;
}

void solver::stop()
{
	wait_for_parameter_update_slot();
	halt = true;
	release_parameter_update_slot();
}

void solver::update_external_data()
{
	give_parameter_update_slot();
	unique_lock<shared_timed_mutex> lock(*data_mutex);
	if(IsConstrObjFunc)
		ext_x = X.head(X.rows() - F.rows());
	else 
		ext_x = X;
	progressed = true;
}

void solver::get_data(VectorXd& X)
{
	unique_lock<shared_timed_mutex> lock(*data_mutex);
	X = ext_x;
	progressed = false;
}

void solver::give_parameter_update_slot()
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

void solver::wait_for_parameter_update_slot()
{
	unique_lock<mutex> lock(*parameters_mutex);
	wait_for_param_update = true;
	while (!params_ready_to_update && is_running)
		param_cv->wait_for(lock, chrono::milliseconds(50));
}

void solver::release_parameter_update_slot()
{
	wait_for_param_update = false;
	param_cv->notify_one();
}