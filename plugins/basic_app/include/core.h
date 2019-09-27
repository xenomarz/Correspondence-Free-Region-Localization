#pragma once

#ifndef RDS_PLUGINS_CORE_H
#define RDS_PLUGINS_CORE_H

#include "app_utils.h"

class Output
{
private:
	
	
	
public:
	//static
	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional
	

	//Solver Button Parameters
	bool solverInitialized;
	

	//regular
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	int ModelID, CoreID;
	

	// Solver thread
	shared_ptr<NewtonSolver> newton;
	shared_ptr<GradientDescentSolver> gradient_descent;
	shared_ptr<solver> solver;
	shared_ptr<TotalObjective> totalObjective;


	//Constructor & initialization
	Output();
	~Output(){}

	void stop_solver_thread(bool& solver_on);
	void start_solver_thread(thread& thread, bool& solver_on);
};

#endif