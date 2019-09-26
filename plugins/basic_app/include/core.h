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



	//regular
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	string name;
	int index;
	float core_size;

	//Constructor & initialization
	Core(int index);
	~Core(){}

	void setName(string mesh_name);
	
};

#endif