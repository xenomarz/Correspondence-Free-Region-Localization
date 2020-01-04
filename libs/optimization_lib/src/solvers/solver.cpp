#include <solvers/solver.h>
#define SAVE_RESULTS_TO_CSV

solver::solver(const bool isConstrObjFunc, const int solverID)
	:
	solverID(solverID),
	parameters_mutex(std::make_unique<std::mutex>()),
	data_mutex(std::make_unique<std::shared_timed_mutex>()),
	param_cv(std::make_unique<std::condition_variable>()),
	num_steps(2147483647),
	IsConstrObjFunc(isConstrObjFunc)
{
#ifdef SAVE_RESULTS_TO_CSV
	//save data in csv files
	std::string path = Utils::RDSPath() + "CSV_Output\\" + "Solver" + std::to_string(solverID) + "\\";
	mkdir((Utils::RDSPath() + "CSV_Output\\").c_str());
	mkdir(path.c_str());
	SearchDirInfo.open(path + "SearchDirInfo.csv");
	solverInfo.open(path + "solverInfo.csv");
	hessianInfo.open(path + "hessianInfo.csv");
#endif
}

solver::~solver() {
#ifdef SAVE_RESULTS_TO_CSV
	//close csv files
	SearchDirInfo.close();
	solverInfo.close();
	hessianInfo.close();
	std::cout << ">> csv " + std::to_string(solverID) + " files has been closed!" << std::endl;
#endif
}

void solver::init(std::shared_ptr<ObjectiveFunction> objective, const Eigen::VectorXd& X0)
{
	this->objective = objective;
	if (IsConstrObjFunc) { //for constraint objective function
		X.resize(X0.rows() + F.rows());
		X.head(X0.rows()) = X0;
		X.tail(F.rows()) = Eigen::VectorXd::Zero(F.rows());
		ext_x = X.head(X.rows() - F.rows());
	}
	else { //for unconstraint objective function
		X = X0;
		ext_x = X;
	}
	internal_init();
}

void solver::setFlipAvoidingLineSearch(Eigen::MatrixX3i & F)
{
	FlipAvoidingLineSearch = true;
	this->F = F;
}

int solver::run()
{
	is_running = true;
	halt = false;
	int steps = 0;
	do {
		run_one_iteration(steps);
	} while ((a_parameter_was_updated || test_progress()) && !halt && ++steps < num_steps);
	is_running = false;
	
	std::cout << ">> solver " + std::to_string(solverID) + " stopped" << std::endl;
	return 0;
}

void solver::run_one_iteration(const int steps) {
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

void solver::saveSolverInfo(int numIteration, std::ofstream& solverInfo) {
	//show only once the objective's function data
	std::shared_ptr<TotalObjective> totalObj = std::dynamic_pointer_cast<TotalObjective>(objective);
	if (!numIteration) {
		if(IsConstrObjFunc)
			solverInfo << "Obj name,weight,Augmented parameter," << std::endl;
		else 
			solverInfo << "Obj name,weight," << std::endl;
		for (auto& obj : totalObj->objectiveList) {
			solverInfo << obj->name << "," << obj->w << ",";
			if(IsConstrObjFunc)
				solverInfo << std::dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << "," ;
			solverInfo << std::endl;
		}
		solverInfo << std::endl << std::endl;

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
		solverInfo << std::endl;
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
		solverInfo << std::endl;
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
		std::shared_ptr<ConstrainedObjectiveFunction> constr = std::dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj);
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
	solverInfo << std::endl;
}

void solver::saveHessianInfo(int numIteration, std::ofstream& hessianInfo) {
	//show only once the objective's function data
	if (!numIteration) {
		std::shared_ptr<TotalObjective> t = std::dynamic_pointer_cast<TotalObjective>(objective);
		hessianInfo << "Obj name,weight,";
		if(IsConstrObjFunc)
			hessianInfo << "Augmented parameter,";
		hessianInfo << std::endl;
		for (auto& obj : t->objectiveList) {
			hessianInfo << obj->name << "," << obj->w << ",";
			if (IsConstrObjFunc)
				hessianInfo << std::dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << ",";
			hessianInfo << std::endl;
		}
		hessianInfo << std::endl;
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
	hessianInfo << ("Round " + std::to_string(numIteration)).c_str() << std::endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			hessianInfo << A.coeff(i, j) << ",";
		}
		
		hessianInfo << "," << g(i) << std::endl;
	}
	hessianInfo << std::endl;
}

void solver::saveSearchDirInfo(int numIteration, std::ofstream& SearchDirInfo) {
	//calculate values in the search direction vector
	int counter;
	double alpha = 0;
	for ( alpha = -1, counter = 0; alpha <= 1; alpha += 0.005, counter++) {
		Eigen::MatrixXd curr_x = X + alpha * p;
		Eigen::VectorXd grad;
		objective->updateX(curr_x);
		objective->gradient(grad,false);
		alfa[counter] = alpha;
		y_value[counter] = objective->value(false);
		y_augmentedValue[counter] = objective->AugmentedValue(false);
		y_gradientNorm[counter] = grad.norm();
	}
	objective->updateX(X);

	//show only once the objective's function data
	if (!numIteration) {
		std::shared_ptr<TotalObjective> t = std::dynamic_pointer_cast<TotalObjective>(objective);
		SearchDirInfo << "Obj name,weight,";
		if (IsConstrObjFunc)
			SearchDirInfo << "Augmented parameter,";
		SearchDirInfo << std::endl;
		for (auto& obj : t->objectiveList) {
			SearchDirInfo << obj->name << "," << obj->w << ",";
			if (IsConstrObjFunc)
				SearchDirInfo << std::dynamic_pointer_cast<ConstrainedObjectiveFunction>(obj)->augmented_value_parameter << ",";
			SearchDirInfo << std::endl;
		}
		SearchDirInfo << std::endl << "Round" << std::endl;
	}
		
	//add the alfa values as one row
	SearchDirInfo << numIteration << ",";
	SearchDirInfo << "alfa,";
	for (int i = 0; i < counter; i++) {
		SearchDirInfo << alfa[i] << ",";
	}
	SearchDirInfo << std::endl;

	//add the total objective's values as one row
	SearchDirInfo << ",value,";
	for (int i = 0; i < counter; i++) {
		SearchDirInfo << y_value[i] << ",";
	}
	SearchDirInfo << std::endl;

	//add the total objective's augmented values as one row
	if (IsConstrObjFunc) {
		SearchDirInfo << ",augmentedValue,";
		for (int i = 0; i < counter; i++) {
			SearchDirInfo << y_augmentedValue[i] << ",";
		}
		SearchDirInfo << std::endl;
	}
	//add the total objective's augmented values as one row
	if (IsConstrObjFunc) {
		SearchDirInfo << ",grad norm,";
		for (int i = 0; i < counter; i++) {
			SearchDirInfo << y_gradientNorm[i] << ",";
		}
		SearchDirInfo << std::endl;
	}
}

void solver::value_linesearch(std::ofstream& SearchDirInfo)
{
	double step_size;
	if (/*FlipAvoidingLineSearch*/false)
	{
		double min_step_to_singularity;
		if (IsConstrObjFunc) {
			auto MatX = Eigen::Map<Eigen::MatrixX2d>(X.head(X.rows() - F.rows()).data(), X.head(X.rows() - F.rows()).rows() / 2, 2);
			Eigen::MatrixXd MatP = Eigen::Map<const Eigen::MatrixX2d>(p.head(X.rows() - F.rows()).data(), p.head(X.rows() - F.rows()).rows() / 2, 2);
			min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(MatX, F, MatP);
		}
		else {
			auto MatX = Eigen::Map<Eigen::MatrixX2d>(X.data(), X.rows() / 2, 2);
			Eigen::MatrixXd MatP = Eigen::Map<const Eigen::MatrixX2d>(p.data(), p.rows() / 2, 2);
			min_step_to_singularity = igl::flip_avoiding::compute_max_step_from_singularities(MatX, F, MatP);
		}
		step_size = std::min(1., min_step_to_singularity*0.8);
	}
	else
		step_size = 1;

	double new_energy = currentEnergy;
	
	int cur_iter = 0; int MAX_STEP_SIZE_ITER = 50;

	while (cur_iter < MAX_STEP_SIZE_ITER)
	{
		Eigen::MatrixXd curr_x = X + step_size * p;

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
			Eigen::MatrixXd curr_x = X + step_size * p;

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
		SearchDirInfo << ",line search type,Gradient norm," << std::endl;
	else
		SearchDirInfo << ",line search type,Function value," << std::endl;
	SearchDirInfo << ",Chosen alfa," << step_size << "," << std::endl;
	SearchDirInfo << ",LineSearch iter," << cur_iter << "," << std::endl;
#endif
}

void solver::gradNorm_linesearch(std::ofstream& SearchDirInfo)
{
	double step_size = 1;
	Eigen::VectorXd grad;

	objective->updateX(X);
	objective->gradient(grad,false);
	double current_GradNrom = grad.norm();
	double new_GradNrom = current_GradNrom;

	int cur_iter = 0; int MAX_STEP_SIZE_ITER = 50;

	while (cur_iter < MAX_STEP_SIZE_ITER)
	{
		Eigen::MatrixXd curr_x = X + step_size * p;

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
			Eigen::MatrixXd curr_x = X + step_size * p;

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
		SearchDirInfo << ",line search type,Gradient norm," << std::endl;
	else
		SearchDirInfo << ",line search type,Function value," << std::endl;
	SearchDirInfo << ",Chosen alfa," << step_size << "," << std::endl;
	SearchDirInfo << ",LineSearch iter," << cur_iter << "," << std::endl;
#endif
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
	std::unique_lock<std::shared_timed_mutex> lock(*data_mutex);
	if(IsConstrObjFunc)
		ext_x = X.head(X.rows() - F.rows());
	else 
		ext_x = X;
	progressed = true;
}

void solver::get_data(Eigen::VectorXd& X)
{
	std::unique_lock<std::shared_timed_mutex> lock(*data_mutex);
	X = ext_x;
	progressed = false;
}

void solver::give_parameter_update_slot()
{
	a_parameter_was_updated = false;
	std::unique_lock<std::mutex> lock(*parameters_mutex);
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
	std::unique_lock<std::mutex> lock(*parameters_mutex);
	wait_for_param_update = true;
	while (!params_ready_to_update && is_running)
		param_cv->wait_for(lock, std::chrono::milliseconds(50));
}

void solver::release_parameter_update_slot()
{
	wait_for_param_update = false;
	param_cv->notify_one();
}