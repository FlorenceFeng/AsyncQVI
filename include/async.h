#ifndef ASYNC_H
#define ASYNC_H

#include <iostream>
#include <thread>
#include <atomic>
#include "algo.h"
#include "oracle.h"
using namespace std;
extern std::atomic<int> iter;
extern pthread_barrier_t barrier; 
// asynchronous worker
void asyncQVI(int thread_id, QVI qvi, Params* params) {
	
	while(!params->stop){
		
		// asynchronous
		qvi.update(iter);
		iter++;
		if(iter > params->threshold){
			pthread_barrier_wait(&barrier);
			if(thread_id == 0){
				cout<<iter<<' ';
				qvi.test();
				params->threshold += params->check_step;
				//params->max_inner_iter = min(params->max_inner_iter*2., 100.);
				//cout<<params->max_inner_iter<<endl;
				if(iter > params->max_outer_iter)
					params->stop = 1;
			}
			pthread_barrier_wait(&barrier);
		}			
	}
	return;
}

void asyncQL(int thread_id, Qlearning ql, Params* params) {
	
	while(!params->stop){
		
		// asynchronous
		ql.update(iter);
		iter++;
		if(iter > params->threshold){
			pthread_barrier_wait(&barrier);
			if(thread_id == 0){
				cout<<iter<<' ';
				ql.test();
				params->threshold += params->check_step;
				if(iter > params->max_outer_iter)
					params->stop = 1;
			}
			pthread_barrier_wait(&barrier);
		}	
	}
	return;
}
#endif