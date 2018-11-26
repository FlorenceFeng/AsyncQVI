#ifndef QVI_H
#define QVI_H

#include <iostream>
#include <vector>
#include "util.h"
#include "so.h"
using namespace std;

extern pthread_mutex_t writelock;

class QVI{
	private: 
		int init_state;
		int init_action;
		int next_state;
		double r;
		double S;
	
	public:
		std::vector<double>* V;
		std::vector<int>* pi;
		Params* params;
	
		QVI(std::vector<double>* V_, std::vector<int>* pi_, Params* params_){
			V = V_;
			pi = pi_;
			params = params_;
			cur_state = 0;
			cur_action = 0;
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
			
			S = 0.;
			for (int i = 0; i < params->max_inner_iter; i++){
				so_sailing(init_state, init_action, next_state, r);
				S += r + params->gamma * V->at(next_state);
			}
			S = S / params->max_inner_iter;
			double newQ = S - (1-params->gamma)*params->epsilon/4;
			
			// update global variables with mutex
			pthread_mutex_lock(&writelock);
			if (newQ > V->at(init_state)){
				V->at(init_state) = newQ;
				pi->at(init_state) = init_action;
			}
			pthread_mutex_unlock(&writelock)
		}
};


#endif

