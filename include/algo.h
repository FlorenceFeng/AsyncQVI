#ifndef QLEARNING_H
#define QLEARNING_H

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "oracle.h"
using namespace std;

extern pthread_mutex_t writelock;

class QVI{
	private: // local variables for each thread
		int init_state;
		int init_action;
		int next_state;
		double r;
		double S;
		Sailing s;
			
	public:  // global variables shared by all threads
		std::vector<double>* V;
		std::vector<int>* pi;
		Params* params;
	
		// constructor
		QVI(std::vector<double>* V_, std::vector<int>* pi_, Params* params_){
			V = V_;
			pi = pi_;
			params = params_;
			init_state = 0;
			init_action = 0;
			s.setValues(params);
		}
		
		// update global variables
		void update(int iter){
			
			// select (state, action) uniformly random
			if(params->style == 0){
				init_state = uniformInt(0, params->len_state-1);
				init_action = uniformInt(0, params->len_action-1);
			}
			// select (state, action) globally cyclic
			else{
				init_state = (iter/params->len_action) % params->len_state;
				init_action = iter % params->len_action;
			}
			
			S = 0.;
			for (int i = 0; i < params->max_inner_iter; i++){
				// call sample oracle
				s.SO(init_state, init_action, next_state, r);
				S += r + params->gamma * V->at(next_state);
			}
			// averaged reward
			S = S / params->max_inner_iter;
			double newQ = S - (1-params->gamma)*params->epsilon/4.;
			
			// update shared memory
			pthread_mutex_lock(&writelock);
			if (newQ > V->at(init_state)){
				V->at(init_state) = newQ;
				pi->at(init_state) = init_action;
			}
			pthread_mutex_unlock(&writelock);
		}
		
		// evaluate current policy
		void test(){
			test_sailing(s, pi, params);
		}
};

class Qlearning {
	
	private: // local variables for each thread
		int init_state = 0;
		int init_action = 0;
		int next_state = 0;
		double r = 0.;
		Sailing s;
		
	public:  // global variables shared by all threads
		std::vector<std::vector<double>>* Q;
		std::vector<double>* V;
		std::vector<int>* pi;
		Params* params;
	
		Qlearning(std::vector<std::vector<double>>* Q_, 
				  std::vector<double>* V_, 
				  std::vector<int>* pi_, 
				  Params* params_){
			Q = Q_;
			V = V_;
			pi = pi_;
			params = params_;
			s.setValues(params);			
		}
		
		// update global variables
		void update(int iter){
			
			// select (state, action) uniformly random
			if(params->style == 0){
				init_state = uniformInt(0, params->len_state-1); 
				init_action = uniformInt(0, params->len_action-1);
			}
			// select (state, action) globally cyclic 
			else if(params->style == 1){
				init_state = (iter/params->len_action) % params->len_state;
				init_action = iter % params->len_action;
			}
			// select (state, action) following a Markovian trajectory with some exploration
			else{
				init_state = next_state;
				init_action = (*pi)[next_state];
				if(uniformDouble(0.,1.) < params->explore)
					init_action = uniformInt(0, params->len_action-1);
			}
			
			// call sample oracle
			s.SO(init_state, init_action, next_state, r);
			
			// update global variables with mutex
			pthread_mutex_lock(&writelock);
			
			// learning rate of Q-learning
			//params->alpha = 1./pow(iter,0.51);
			(*Q)[init_state][init_action] = (1-params->alpha) * (*Q)[init_state][init_action]
											+ params->alpha * (r + params->gamma*(*V)[next_state]);
			if((*Q)[init_state][init_action] > (*V)[init_state]){
				(*V)[init_state] = (*Q)[init_state][init_action];
				(*pi)[init_state] = init_action;
			}
			pthread_mutex_unlock(&writelock);			
		}
		
		// evaluate current policy
		void test(){
			test_sailing(s, pi, params);
		}
};

class VRVI{
	private:
		int init_state = 0;
		int init_action = 0;
		int next_state = 0;
		double r = 0.;
		double temp = 0.;
		Sailing s;
	
	public:
		std::vector<std::vector<double>>* x;
		std::vector<double>* v_outer;
		std::vector<double>* v_inner;
		std::vector<int>* pi;
		Params* params;
	
		VRVI(std::vector<std::vector<double>>* x_, 
						 std::vector<double>* v_outer_,
						 std::vector<double>* v_inner_,
						 std::vector<int>* pi_,
						 Params* params_){
				x = x_;
				v_outer = v_outer_;
				v_inner = v_inner_;
				pi = pi_;
				params = params_;
				s.setValues(params);			
		}
	
		void solve(){
			srand (time(NULL));
			for(int t = 0; t < params->max_outer_iter; t++){
				
				// approximate x
				for(int i = 0; i < params->len_state; i++){
					for(int a = 0; a < params->len_action; a++){
						(*x)[i][a] = 0;
						for(int n = 0; n < params->sample_num_1; n++){
							s.SO(i, a, next_state, r);
							(*x)[i][a] += (*v_outer)[next_state];
						}
						(*x)[i][a] /= params->sample_num_1;
					}
				}
				
				// RandomizedVI
				for(int k = 0; k < params->max_inner_iter; k++){
					// APXVAL
					for(int i = 0; i < params->len_state; i++){
						for(int a = 0; a < params->len_action; a++){
							temp = 0.;
							for(int n = 0; n < params->sample_num_2; n++){
								s.SO(i, a, next_state, r);
								temp += r + params->gamma * ((*v_inner)[next_state]-(*v_outer)[next_state]);
							}
							temp = temp/params->sample_num_2 + params->gamma * (*x)[i][a]
								- 2*params->gamma*params->epsilon;
							if (temp > (*v_inner)[i]){
								(*v_inner)[i] = temp;
								(*pi)[i] = a;
							}
						}
					}
				}
				
				// reset parameters. The resetting fashion is tunable
				params->epsilon /= 2.;
				params->sample_num_1 *= 4;
				params->sample_num_2 *= 4;
				
				*v_outer = *v_inner;
				if(t % params->check_step==0){
					test_sailing(s, pi, params);
				}
		
			}
		}
		
};

class VRQVI{
	private:
		int init_state = 0;
		int init_action = 0;
		int next_state = 0;
		double r = 0.;
		double temp = 0.;
		double v_outer_max = 0.;
		Sailing s;
	
	public:
		std::vector<std::vector<double>>* Q;
		std::vector<std::vector<double>>* w;
		std::vector<double>* v_outer;
		std::vector<double>* v_inner;
		std::vector<int>* pi;
		Params* params;
	
		VRQVI(std::vector<std::vector<double>>* Q_, 
			 std::vector<std::vector<double>>* w_, 
						 std::vector<double>* v_outer_,
						 std::vector<double>* v_inner_,
						 std::vector<int>* pi_,
						 Params* params_){
				Q = Q_;
				w = w_;
				v_outer = v_outer_;
				v_inner = v_inner_;
				pi = pi_;
				params = params_;
				s.setValues(params);			
		}
	
		void solve(){
			srand (time(NULL));
			for(int t = 0; t < params->max_outer_iter; t++){
				
				// max element of v_fix
				v_outer_max = fabs((*v_outer)[0]);
				for(int index = 1; index < params->len_state; index++){
					if(fabs((*v_outer)[index]) > v_outer_max)
						v_outer_max = fabs((*v_outer)[index]);
				}
				
				// compute a coarse estimate of Q
				for(int i = 0; i < params->len_state; i++){
					for(int a = 0; a < params->len_action; a++){
						double v_sum = 0;
						double v_square_sum = 0;
						double r_sum = 0;
						for(int n = 0; n < params->sample_num_1; n++){
							s.SO(i, a, next_state, r);
							v_sum += (*v_outer)[next_state];
							v_square_sum += pow((*v_outer)[next_state],2);
							r_sum += r;
						}
						double v_ave = v_sum / params->sample_num_1;
						double v_square_ave = v_square_sum / params->sample_num_1;
						double r_ave = r_sum / params->sample_num_1;
						(*w)[i][a] = v_ave - sqrt(2*params->alpha1*(v_square_ave-v_ave))
						            - (4*pow(params->alpha1,0.75) + 2/3*params->alpha1)*v_outer_max;
						(*Q)[i][a] = r_ave + params->gamma * (*w)[i][a];
						
					}
				}
				
			    // improve Q 
				for(int k = 0; k < params->max_inner_iter; k++){				
					// compute the estimate of P(v_inner - v_outer)
					for(int i = 0; i < params->len_state; i++){
						// update v and pi
						if((*v_inner)[i] < *max_element(((*Q)[i]).begin(), ((*Q)[i]).end())){
							(*v_inner)[i] = *max_element(((*Q)[i]).begin(), ((*Q)[i]).end());
							(*pi)[i] =  distance(((*Q)[i]).begin(), max_element(((*Q)[i]).begin(), ((*Q)[i]).end()));
						}
					}
				
					for(int i = 0; i < params->len_state; i++){
						for(int a = 0; a < params->len_action; a++){
							double g = 0.;
							for(int n = 0; n < params->sample_num_2; n++){
								s.SO(i, a, next_state, r);
								g += r + params->gamma * ((*v_inner)[next_state]-(*v_outer)[next_state]);
							}
							(*Q)[i][a] = g/params->sample_num_2 -(1-params->gamma)*params->epsilon/8.
							             + params->gamma * (*w)[i][a];					
						}
					}
				}
				// reset parameters. The resetting fashion is tunable
				params->epsilon /= 2.;
				params->sample_num_1 *= 2; 
				params->sample_num_2 *= 2; 
				
				*v_outer = *v_inner;
				if(t % params->check_step==0){
					test_sailing(s, pi, params);
				}
			}
			return;
		}
};
	
#endif
