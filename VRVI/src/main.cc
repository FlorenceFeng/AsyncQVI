#include <iostream>
#include <fstream>
#include <vector>
#include "util.h"
#include "so.h"
using namespace std;

int main(int argc, char* argv[]){
	
	// Step 0: set parameters
	Params params;
	parse_input_argv(&params, argc, argv);
	
	// Step 1: initialize variables
	std::vector<std::vector<double>> x(params.len_state, std::vector<double>(params.len_action, 0.));
	std::vector<double> v_outer(params.len_state, 0.);
	std::vector<double> v_inner(params.len_state, 0.);
	std::vector<int> pi(params.len_state, 0.);
	int next_state = 0;
	double r = 0.;
	double temp = 0.;
	
	// Step 2: launch training
	for(int t = 0; t < params.max_outer_loop; t++){
		
		// approximate x
		for(int i = 0; i < params.len_state; i++){
			for(int a = 0; a < params.len_action; a++){
				for(int s = 0; s < params.sample_num; s++){
					so_sailing(i, a, next_state, r);
					x[i][a] += params.gamma * v_outer[next_state];
				}
				x[i][a] /= params.sample_num;
			}
		}
		
		// update v
		for(int k = 0; k < params.max_inner_loop; k++){
			for(int i = 0; i < params.len_state; i++){
				for(int a = 0; a < params.len_action; a++){
					temp = 0.;
					for(int s = 0; s < params.sample_num; s++){
						so_sailing(i, a, next_state, r);
						temp += r + params.gamma * (v_inner[next_state]-v_outer[next_state]);
					}
					temp = temp/params.sample_num + x[i][a];
					if (temp > v_inner[i]){
						v_inner[i] = temp;
						pi[i] = a;
					}
				}
			}
		}
		
		v_outer = v_inner;
	}
	
	// Step 3: save policy
	std::ofstream outFile("policy.txt");
    for (const auto &e : pi) outFile << e << "\n";
    outFile.close();
	return 0;
}
		
					
		
		
		
		
		
		
		
		
		
		