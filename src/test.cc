#include <iostream>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <vector>
#include <cmath>
#include "async.h"
using namespace std;

std::atomic<int> iter(0);
pthread_mutex_t writelock;

int main(int argc, char** argv){
	
	// Step 0: set parameters
	Params params;
	parse_input_argv(&params, argc, argv);
	
	// Step 1: choose an algorithm, 0 is AsyncQVI, 1 is Qlearning, 2 is VRVI
	double time = get_wall_time(); 
	if(params.algo == 0){
		std::vector<double> V(params.len_state, 0.); 
		std::vector<int> pi(params.len_state, 0.);
		QVI obj(&V, &pi, &params);
		std::vector<std::thread> mythreads;
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads.push_back(std::thread(async, i, obj, &params));
		} 
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads[i].join();
		}
	}
	else if(params.algo == 1){		
		std::vector<std::vector<double>> Q(params.len_state, std::vector<double>(params.len_action, 0.));
		std::vector<double> V(params.len_state);
		std::vector<int> pi(params.len_state);
		Qlearning obj(&Q, &V, &pi, &params);
		std::vector<std::thread> mythreads;
		double time = get_wall_time();
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads.push_back(std::thread(async, i, obj, &params));
		} 
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads[i].join();
		}
	}
	else{
		std::vector<std::vector<double>> x(params.len_state, std::vector<double>(params.len_action, 0.));
		std::vector<double> v_outer(params.len_state, 0.);
		std::vector<double> v_inner(params.len_state, 0.);
		std::vector<int> pi(params.len_state, 0.);
		VRVI obj(&x, &v_outer, &v_inner, &pi, &params); 
		obj.solve();
	}
	cout<<"Time is: "<<get_wall_time() - time<<endl;
	
	// Step 2: test policy and save results
	test_sailing(&pi);
	std::ofstream outFile("policy.txt");
    for (const auto &e : pi) outFile << e << "\n";
    outFile.close();
	return 0;
}
			