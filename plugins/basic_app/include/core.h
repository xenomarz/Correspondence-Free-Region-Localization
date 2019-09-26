#pragma once

#ifndef RDS_PLUGINS_CORE_H
#define RDS_PLUGINS_CORE_H

#include "app_utils.h"

class Core
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
	string name;
	int index;
	float core_size;

	// Solver thread
	shared_ptr<NewtonSolver> newton;
	shared_ptr<GradientDescentSolver> gradient_descent;
	shared_ptr<solver> solver;
	shared_ptr<TotalObjective> totalObjective;


	//Constructor & initialization
	Core(int index);
	~Core(){}

	void setName(string mesh_name);
	
};

#endif