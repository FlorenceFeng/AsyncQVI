#include <iostream>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <vector>
#include "async.h"
using namespace std;

std::atomic<int> iter(0);
pthread_mutex_t writelock;

int main(int argc, char** argv){
	
	// Step 0: set parameters
	Params params;
	parse_input_argv(&params, argc, argv);
	
	// Step 1: initialize global variables
	std::vector<std::vector<double>> Q(params.len_state, std::vector<double>(params.len_action, 0.));
	std::vector<double> V(params.len_state);
	std::vector<int> pi(params.len_state);
	Qlearning qlearning(&Q, &V, &pi, &params);
	std::vector<std::thread> mythreads;
	
	// Step 2: launch asynchronous running
	double time = get_wall_time();
	for (size_t i = 0; i < params.total_num_threads; i++) {
		mythreads.push_back(std::thread(async, i, qlearning, &params));
	} 
	for (size_t i = 0; i < params.total_num_threads; i++) {
		mythreads[i].join();
	}
	cout<<"Time is: "<<get_wall_time() - time<<endl;
	
	// Step 3: save results
	std::ofstream outFile("policy.txt");
    for (const auto &e : pi) outFile << e << "\n";
    outFile.close();
	return 0;
}
			