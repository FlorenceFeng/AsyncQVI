#include <iostream>
#include <thread>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "qvi.h"
#include "util.h"
using namespace std;

int main(int argc, char* argv[]){
	
	// Step 0: set parameters
	Params params;
	parse_input_argv(&params, argc, argv);
	
	// Step 1: initialize global variables
	gsl_vector* V = gsl_matrix_calloc(params.len_state);
	gsl_vector* pi = gsl_matrix_calloc(params.len_state);
	
	// Step 2: launch parallel threads
	QVI qvi(V, &params);
	std::vector<std::thread> mythreads;
	params->time = get_wall_time();
	for (size_t i = 0; i < params.total_num_threads; i++) {
		mythreads.push_back(std::thread(async, i, bcd, &params));
	} 
	for (size_t i = 0; i < params.total_num_threads; i++) {
		mythreads[i].join();
	}
	params->time = get_wall_time() - params->time;
	
	// Step 3: save policy
	save_policy(pi);

	// Step 4: free memory
	gsl_vector_free(V);
	gsl_vector_free(pi);
	
	return 0;
}
	