#include <iostream>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <atomic>
#include <vector>
#include <cmath>
#include "async.h"
#include "algo.h"
#include "oracle.h"
using namespace std;

std::atomic<int> iter(1);
pthread_mutex_t writelock;
pthread_barrier_t barrier; 
pthread_barrierattr_t attr;
int main(int argc, char** argv){
	
	// Step 0: set parameters
	Params params;
	parse_input_argv(&params, argc, argv);
	
	// Step 1: choose an algorithm, 0 is AsyncQVI, 1 is Qlearning, 2 is VRVI
	params.time = get_wall_time(); 
	std::vector<int> pi(params.len_state, 0.);
	pthread_barrier_init(&barrier, &attr, params.total_num_threads);
	cout<<"iter time reward flag"<<endl;
	if(params.algo == 0){
		std::vector<double> V(params.len_state, 0.); 
		QVI obj(&V, &pi, &params);
		std::vector<std::thread> mythreads;
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads.push_back(std::thread(asyncQVI, i, obj, &params));
		} 
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads[i].join();
		}
	}
	else if(params.algo == 1){		
		std::vector<std::vector<double>> Q(params.len_state, std::vector<double>(params.len_action, 0.));
		std::vector<double> V(params.len_state);
		Qlearning obj(&Q, &V, &pi, &params);
		std::vector<std::thread> mythreads;
		double time = get_wall_time();
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads.push_back(std::thread(asyncQL, i, obj, &params));
		} 
		for (size_t i = 0; i < params.total_num_threads; i++) {
			mythreads[i].join();
		}
	}
	else{
		std::vector<std::vector<double>> x(params.len_state, std::vector<double>(params.len_action, 0.));
		std::vector<double> v_outer(params.len_state, 0.);
		std::vector<double> v_inner(params.len_state, 0.);
		VRVI obj(&x, &v_outer, &v_inner, &pi, &params); 
		obj.solve();
	}
	
	// Step 2: save results
	if(params.save){
		std::ofstream outFile("policy.txt");
		for (int i = 0; i < params.len_state; i++){
			outFile << pi[i] << "\n";
		}
		outFile.close();
	}
	return 0;
}
			