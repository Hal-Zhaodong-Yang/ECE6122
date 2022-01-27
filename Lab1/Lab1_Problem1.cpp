/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 09/24/2021
Description:
This file is to calculate the prime factor of a number. By mod the number with every odd number less
than sqrt(ulInputNumber) and 2, this file is able to find every prime factor.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
using namespace std;

//Get the prime factors
bool GetPrimeFactors (const unsigned long ulInputNumber, string &strOutput)
{
    
    unsigned long n = ulInputNumber;
    if(n <= 1) return false;
    while(n % 2 == 0)
    {
        strOutput = strOutput + "2,";
        n = n / 2;
    }

    for(int prime_factor = 3; prime_factor < sqrt(ulInputNumber); prime_factor = prime_factor + 2)
    {
        while(n % prime_factor == 0)
        {
            strOutput = strOutput + to_string(prime_factor) + ",";
            n = n / prime_factor;
        }
    }

    if(n != 1) strOutput = strOutput + to_string(n) + ",";

    return true;


}

int main(int argc, char* argv[])
{
    ofstream myfile;
    myfile.open("output1.txt");
    if(argc < 2)
    {
        cout << "please type in the number";
        return 1;
    }

    for(int i = 1; i < argc; i ++)
    {
        string strOutput = "";
        if(GetPrimeFactors(stoll(argv[i]), strOutput)){
            strOutput.pop_back();
            myfile << strOutput << "\n";
        }
        else{
            myfile << "No prime factors\n";
        }
    }

    myfile.close();

    return 0;


}

