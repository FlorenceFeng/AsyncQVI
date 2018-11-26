#ifndef QLEARNING_H
#define QLEARNING_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "util.h"
#include "so.h"
using namespace std;

extern pthread_mutex_t writelock;

class Qlearning {
	
	private:
		int init_state = 0;
		int init_action = 0;
		int next_state = 0;
		double r = 0.;
		std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution(0.0,1.0);
		
	public:
		std::vector<std::vector<double>>* Q;
		std::vector<double>* V;
		std::vector<int>* pi;
		Params* params;
	
		Qlearning(std::vector<std::vector<double>>* Q_, std::vector<double>* V_, std::vector<int>* pi_, Params* params_){
			Q = Q_;
			V = V_;
			pi = pi_;
			params = params_;
		}
		
		void update(int iter){
			
			if(params->style == 0){
				// random update
				init_state = randnum(0, params->len_state-1);
				init_action = randnum(0, params->len_action-1);
			}
			else if(params->style == 1){
				// cyclic update
				init_state = iter % params->len_state;
				init_action = iter % params->len_action;
			}
			else{
				// Markovian update
				init_state = next_state;
				init_action = (*pi)[next_state];
				if(distribution(generator) < params->epsilon)
					init_action = randnum(0, len_action-1);
			}
			
			// call for a sample
			so_sailing(init_state, init_action, next_state, r);
			
			// update global variables with mutex
			pthread_mutex_lock(&writelock);
			(*Q)[init_state][init_action] = (1-alpha) * (*Q)[init_state][init_action]
											+ alpha * (r + params->gamma*(*V)[next_state]);
			if((*Q)[init_state][init_action] > (*V)[init_state]){
				(*V)[init_state] = (*Q)[init_state][init_action];
				(*pi)[init_state] = init_action;
			}
			pthread_mutex_unlock(&writelock);
			
		}
	}
	
#endif