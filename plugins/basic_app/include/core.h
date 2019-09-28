#pragma once

#ifndef RDS_PLUGINS_CORE_H
#define RDS_PLUGINS_CORE_H

#include "app_utils.h"

class Output
{	
public:
	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	int ModelID, CoreID;
	ImVec2 window_position, window_size;
	
	// Solver thread
	shared_ptr<NewtonSolver> newton;
	shared_ptr<GradientDescentSolver> gradient_descent;
	shared_ptr<solver> solver;
	shared_ptr<TotalObjective> totalObjective;

	//Constructor & initialization
	Output() {
		// Initialize solver thread
		newton = make_shared<NewtonSolver>();
		gradient_descent = make_shared<GradientDescentSolver>();
		solver = newton;
		totalObjective = make_shared<TotalObjective>();
	}
	~Output(){}
};

#endif