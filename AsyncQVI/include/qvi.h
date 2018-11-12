#ifndef QVI_H
#define QVI_H

#include <iostream>
#include <gsl/matrix.h>
#include <gsl/vector.h>
#include "util.h"
#include "so.h"
using namespace std;

class QVI{
	private: 
		int cur_state;
		int cur_action;
		int next_state;
		double r;
		double S;
	
	public:
		gsl_vector* V;
		gsl_vector* pi;
		Params* params;
	
	
		QVI(gsl_vector* V_, Params* params_){
			V = V_;
			params = params_;
			cur_state = 0;
			cur_action = 0;
		}
		
		void select(){
			cur_state = randnum(0, len_state-1);
			cur_action = randnum(0, len_action-1);
		}
		
		void update(int iter){
			select();
			cout<<cur_state<<endl;
			S = 0;
			for (int i = 0; i < params->max_inner_iter; i++){
				so(cur_state, cur_action, next_state, r);
				S += r + gamma * gsl_vector_get(V, next_state)
			}
			S = S / params->max_inner_iter;
			double newQ = S - (1-params->gamma)*parmas->epsilon/4;
			
			if (newQ > gsl_matrix_get(V, cur_state)){
				gsl_matrix_set(V, cur_state, newQ);
				gsl_matrix_set(pi, cur_state, cur_action);
			}
		}
};


#ENDIF

