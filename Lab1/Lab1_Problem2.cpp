/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 09/24/2021
Description:
This file is to calculate Langton’s Ant problem. Initializing a 2D bool array "map" with size 2000*2000 representing the
grid. Then put the ant in the center (1000, 1000) of it. Let the ant move by its rules and flip the color of the square.
Then, because the ant traces a pseudo-random path until around 10,000 steps. The ant starts building a recurrent
 "highway" pattern of 104 steps that repeats indefinitely. So we can record how many black squares was produced
 in 104 steps after about 10000 steps. After we record the black squares produced in this 104 steps, we can
 break the loop and calculate how many black squares was produced during the rest steps.
*/

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

bool map[2000][2000] = {false};

int main(int argc, char* argv[])
{

    int statex = 1000;
    int statey = 1000;

    int direction = 0; //0 for up, 1 for right, 2 for down, 3 for left

    const unsigned long steps = stoll(argv[1]);

    long long black_count = 0;

    unsigned long repeat;

    int flag;

    for(int i = 0; i < steps; i ++){
        if(map[statex][statey]){
            map[statex][statey] = false;
            direction = (direction + 3) % 4;
            black_count --;
            flag = 0;

        }else{
            map[statex][statey] = true;
            direction = (direction + 1) % 4;
            black_count ++;
            flag = 1;

        }
        switch(direction){
            case 0:
                statey --;
                break;
            case 1:
                statex ++;
                break;
            case 2:
                statey ++;
                break;
            case 3:
                statex --;
                break;
            default:
                cout << "out of direcion range\n";
        }

        if(i == 15600 + steps % 104 - 1){
            repeat = black_count;
        }
        if(i == 15704 + steps % 104 - 1){
            repeat = black_count - repeat;
            break;
        }
    }

    if(steps >= 15704 + steps % 104){
        black_count = black_count + (steps - 15704 - steps % 104) / 104 * repeat;
    }


    ofstream myfile;
    myfile.open("output2.txt");
    myfile << to_string(black_count);
    myfile.close();

    //process.close();

    



    return 0;

}