#ifndef ALGO_H
#define ALGO_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "oracle.h"
using namespace std;
extern pthread_mutex_t writelock;

class QVI{
	private: 
		int init_state;
		int init_action;
		int next_state;
		double r;
		double S;
		Sailing s;
	
	public:
		std::vector<double>* V;
		std::vector<int>* pi;
		Params* params;
	
		QVI(std::vector<double>* V_, std::vector<int>* pi_, Params* params_){
			V = V_;
			pi = pi_;
			params = params_;
			init_state = 0;
			init_action = 0;
			s.setValues(params);
		}
		
		void update(int iter){
			//cout<<iter<<endl;
			if(params->style == 0){
				// random update
				init_state = randint(0, params->len_state-1);
				init_action = randint(0, params->len_action-1);
			}
			else if(params->style == 1){
				// cyclic update
				init_state = (iter/params->len_action) % params->len_state;
				init_action = iter % params->len_action;
			}
			else{
				// Markovian update
				init_state = next_state;
				init_action = (*pi)[next_state];
				if(randdouble(0.,1.) < params->epsilon)
					init_action = randint(0, params->len_action-1);
			}
			
			S = 0.;
			for (int i = 0; i < params->max_inner_iter; i++){
				s.SO(init_state, init_action, next_state, r);
				//cout<<init_state<<' '<<init_action<<' '<<next_state<<' '<<r<<endl;
				S += r + params->gamma * V->at(next_state);
			}
			S = S / params->max_inner_iter;
			double newQ = S - (1-params->gamma)*params->epsilon/4;
			
			
			pthread_mutex_lock(&writelock);
			if (newQ > V->at(init_state)){
				V->at(init_state) = newQ;
				pi->at(init_state) = init_action;
			}
			pthread_mutex_unlock(&writelock);
		}
		
		void test(){
			test_sailing(s, pi, params);
		}
};

class Qlearning {
	
	private:
		int init_state = 0;
		int init_action = 0;
		int next_state = 0;
		double r = 0.;
		Sailing s;
		
	public:
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
		
		void update(int iter){
			
			// random update
			if(params->style == 0){
				init_state = randint(0, params->len_state-1); 
				init_action = randint(0, params->len_action-1); 
			}
			// global cyclic update
			else if(params->style == 1){
				init_state = (iter/params->len_action) % params->len_state;
				init_action = iter % params->len_action;
			}
			// Markovian update
			else{
				init_state = next_state;
				init_action = (*pi)[next_state];
				if(randdouble(0.,1.) < params->epsilon)
					init_action = randint(0, params->len_action-1);
			}
			
			// call for a sample
			s.SO(init_state, init_action, next_state, r);
			
			// update global variables with mutex
			pthread_mutex_lock(&writelock);
			params->alpha = 1./pow(iter,0.5);
			(*Q)[init_state][init_action] = (1-params->alpha) * (*Q)[init_state][init_action]
											+ params->alpha * (r + params->gamma*(*V)[next_state]);
			if((*Q)[init_state][init_action] > (*V)[init_state]){
				(*V)[init_state] = (*Q)[init_state][init_action];
				(*pi)[init_state] = init_action;
			}
			pthread_mutex_unlock(&writelock);			
		}
		
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
			for(int t = 0; t < params->max_outer_iter; t++){
				
				// approximate x
				
				for(int i = 0; i < params->len_state; i++){
					for(int a = 0; a < params->len_action; a++){
						(*x)[i][a] = 0;
						for(int n = 0; n < params->sample_num; n++){
							s.SO(i, a, next_state, r);
							(*x)[i][a] += params->gamma * (*v_outer)[next_state];
						}
						(*x)[i][a] /= params->sample_num;
					}
				}
				
				// RandomizedVI
				for(int k = 0; k < params->max_inner_iter; k++){
					// APXVAL
					for(int i = 0; i < params->len_state; i++){
						for(int a = 0; a < params->len_action; a++){
							temp = 0.;
							for(int n = 0; n < params->sample_num; n++){
								s.SO(i, a, next_state, r);
								temp += r + params->gamma * ((*v_inner)[next_state]-(*v_outer)[next_state]);
							}
							temp = temp/params->sample_num + (*x)[i][a];
							if (temp > (*v_inner)[i]){
								(*v_inner)[i] = temp;
								(*pi)[i] = a;
							}
						}
					}
				}
				
				*v_outer = *v_inner;
				if(t%params->check_step==0){
					test_sailing(s, pi, params);
				}
		
			}
		}
		
};
	
#endif