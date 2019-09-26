#pragma once

#ifndef RDS_PLUGINS_CORE_H
#define RDS_PLUGINS_CORE_H

#include "app_utils.h"

class Core
{
private:
	
	
	
public:
	float Max_Distortion;
	string name;
	int index;
	float core_size;

	//Constructor & initialization
	Core(int index);
	~Core(){}

	void setName(string mesh_name);
	
};

#endif