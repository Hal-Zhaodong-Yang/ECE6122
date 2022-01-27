/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 2021/10/13
Description:
The program is to calculate the integration of a function using the midpoint rule. And OpenMP is used to speed up the
program using multithreading.
*/

#include<stdio.h>
#include<cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <omp.h>


double my_function(double x);

int main(int argc, char* argv[])
{
	std::ofstream myfile;
	myfile.open("Lab2Prob2.txt");
	
	double sup = log(2) / 7;
	const unsigned long long segs = std::stoll(argv[1]);
	double integration = 0;
	double mid;
#pragma omp parallel for
	for (long long i = 0; i < segs; i++) {
		mid = sup / segs * (i + i + 1) / 2;
#pragma omp critical
		{
			integration += my_function(mid) * sup / segs;
		}
	}
	myfile.setf(std::ios::fixed);

	myfile << std::setprecision(6) << integration << "\n";

	myfile.close();


	return 0;
}

double my_function(double x)
{
	double y;
	y = 14 * exp(7 * x);

	return y;
}