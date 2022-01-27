#include <thread>
#include <iostream>
#include <random>
#include <fstream>
#include <iomanip>
#include <vector>
#include <mutex>
#include <ctime>
void expected_steps(int store_idx,int t_num);

std::vector<double> step_avg;
std::mutex vector_mutex;

int main()
{

	int time_now = time(NULL);
	int time_period;
	
	const int processor_count = std::thread::hardware_concurrency();

	for (int i = 0; i < processor_count - 1; i++) {
		step_avg.push_back(0);
	}

	std::vector<std::thread> t;

	for (int i = 0; i < processor_count - 1; i++) {
		t.push_back(std::thread(expected_steps, i, processor_count));
	}
	for (auto single_t = t.begin(); single_t != t.end(); ++single_t) {
		single_t->join();
	}


	//printf("multithread in process...\n");


	

	//printf("processor_count: %d\n", processor_count);

	double average = 0;
	for (int i = 0; i < processor_count - 1; i++) {
		average += step_avg[i];
		//printf("%lf ", step_avg[i]);
	}
	//printf("\n");
	average = average / (processor_count - 1);
	std::ofstream myfile;
	myfile.open("ProblemOne.txt");
	myfile.setf(std::ios::fixed);
	myfile << "Number of threads created: " << processor_count << "\n";
	myfile << "Expected number of steps: " << std::setprecision(6) << average << "\n";
	myfile << "Total number of runs needed for solution convergence: " << "10000000";

	myfile.close();

	//printf("the steps is %lf\n", average);


	time_period = time(NULL) - time_now;

	std::cout << "running time: " << time_period << "\n";


	return 0;

}

void expected_steps(int store_idx, int t_num)
{
	std::random_device rd;
	int runs = 10000000 / t_num;
	int count = 0;
	for (int i = 0; i < runs; i++) {
		//initialize the grid map
		bool grid[5][5];
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 5; j++) {
				grid[i][j] = 0;
			}
		}
		for (int j = 0; j < 5; j++) {
			grid[4][j] = 1;
		}

		//ant's location, direction{0, 1, 2, 3}
		int loc[2] = { 2, 2 };
		int last_loc[2] = { 2, 2 };
		int dir = 0;
		bool loaded = false;
		int step_count = 0;

		//represents if the ant finally stops
		bool end = false;
		while (!end) {
			last_loc[0] = loc[0];
			last_loc[1] = loc[1];
			dir = rd() % 4;
			switch (dir)
			{
			case 0: 
				loc[0] --;
				break;
			case 1:
				loc[1] ++;
				break;
			case 2:
				loc[0] ++;
				break;
			case 3:
				loc[1] --;
				break;
			}
			//if out of range
			if (loc[0] < 0 || loc[0] > 4 || loc[1] < 0 || loc[1] > 4) {
				loc[0] = last_loc[0];
				loc[1] = last_loc[1];
				continue;
			}
			//load a seed if not loaded
			if (grid[loc[0]][loc[1]] && loc[0] == 4 && !loaded) {
				grid[loc[0]][loc[1]] = 0;
				loaded = true;
			}
			//put down a seed if loaded and reach the upper row
			if (!grid[loc[0]][loc[1]] && loc[0] == 0 && loaded) {
				grid[loc[0]][loc[1]] = 1;
				loaded = false;
			}

			end = (grid[0][0] && grid[0][1] && grid[0][2] && grid[0][3] && grid[0][4]);
			step_count++;

			//for checking the movement of the ant and the grid situation
			/*
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					if (i == loc[0] && j == loc[1]) {
						printf("2 ");
						continue;
					}
					printf("%d ", grid[i][j]);
				}
				printf("\n");
			}
			printf("count = %d\n", count);
			printf("\n");
			*/

		}

		vector_mutex.lock();
		step_avg[store_idx] += 1.0 * step_count / runs;
		vector_mutex.unlock();
		
	}

}