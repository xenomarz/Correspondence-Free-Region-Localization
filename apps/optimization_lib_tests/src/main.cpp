#include "../../../libs/optimization_lib/include/utils.h"
#include <iostream>
#include <fstream>
using namespace std;
int main(int argc, char * argv[])
{
	std::ofstream myfile;
	myfile.open("example.csv");

	int counter;
	double alfa = 0;
	myfile << "Round" << endl;


	myfile << "0,";
	myfile << "alfa = ,";
	for (alfa = -3, counter = 0; alfa <= 3; alfa += 0.01, counter++) {
		myfile << alfa << ",";
	}
	myfile << endl;

	myfile << ",beta = ,";
	for (alfa = -3, counter = 0; alfa <= 3; alfa += 0.01, counter++) {
		myfile << alfa*alfa << ",";
	}
	myfile << endl;
	
	myfile << ",ghstamma = ,";
	for (alfa = -3, counter = 0; alfa <= 3; alfa += 0.01, counter++) {
		myfile << alfa * 2 << ",";
	}
	myfile << endl;
	
	//myfile.close();
		
	
	return 0;
}
