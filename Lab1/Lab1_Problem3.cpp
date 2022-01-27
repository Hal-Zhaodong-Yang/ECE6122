/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 09/24/2021
Description:
This file is trying to investigate multiple reflections of a laser beam in a "white cell". 
Because the white cell is symmetric, we just need to consider the initial reflection’s x location positive.
Instead of reflecting the laser beam. I reflect the triangle by its side, and keep the laser beam line constant,
which is completely the same when calculating how many times the laser beam reflected on the mirror.
Then the only thing I need to do is find the locations of the reflected point C, which could be describle as:
(60 * k, 20 * sqrt(3)* n) and (60 * k + 30, 20 * sqrt(3) * n + 10 * sqrt(3)), [n is unsigned long, k is unsigned long]
Then we need to calculate which is the first C point the laser beam line joins. And we can tell how many times the
laser beams has actually reflected by the y location of the C point. Every time the laser beam rises 10 * sqrt(3) higher
in y axis the laser beam reflect 2 times except the first 10 * sqrt(3) and the last.
*/


#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

int main(int args, char *argv[])
{
    //the input x value
    double x;
    x = stod(argv[1]);
    x = abs(x);
    
    long long level = 0;  //represents how many 10 * sqrt(3) higher the laser beam has rised in y axis
    long double sqrt3 = sqrt(3);
    
    //reflection loop
    while(1){

        level ++;
        
        //decide if the laser beam has passed through a point C with odd "level"
        if(fmod(x * level + 30, 60) < 0.01 / sqrt3 ){
            break;
        }
        if((60 - fmod(x * level + 30, 60)) < 0.01 / sqrt3 ){
            break;
        }

        level ++;
        //decide if the laser beam has passed through a point C with even "level"
        if(fmod(x * level, 60) < 0.01 / sqrt3 ){
            break;
        }
        if((60 - fmod(x * level, 60)) < 0.01 / sqrt3 ){
            break;
        }

    }

    ofstream myfile;
    myfile.open("output3.txt");
    myfile << (level - 2) * 2 + 1;  //calculate the final reflection time by "level"
    myfile.close();

    return 0;

}