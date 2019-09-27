#pragma once

#ifndef RDS_PLUGINS_CORE_H
#define RDS_PLUGINS_CORE_H

#include "app_utils.h"

class Output
{
private:
	
	
	
public:
	//static
	
	float Max_Distortion;
	Vector3f 
		Highlighted_face_color, 
		Fixed_face_color, 
		Fixed_vertex_color, 
		model_color, 
		Dragged_face_color, 
		Dragged_vertex_color, 
		Vertex_Energy_color, 
		text_color;
	set<int> selected_faces, selected_vertices;
	app_utils::Parametrization param_type;
	float texture_scaling_output;
	bool Highlighted_face;

	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional
	

	//Solver Button Parameters
	bool solver_on, solverInitialized, show_text;
	app_utils::Distortion distortion_type;
	app_utils::SolverType solver_type;

	//regular
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	int ModelID, CoreID;
	float core_size;

	// Solver thread
	shared_ptr<NewtonSolver> newton;
	shared_ptr<GradientDescentSolver> gradient_descent;
	shared_ptr<solver> solver;
	shared_ptr<TotalObjective> totalObjective;


	//Constructor & initialization
	Output();
	~Output(){}

	void stop_solver_thread();
	void start_solver_thread(thread& thread);
};

#endif