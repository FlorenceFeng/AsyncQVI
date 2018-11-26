#ifndef ASYNC_H_
#define ASYNC_H_

#include <iostream>
#include <thread>
#include "qvi.h"
using namespace std;

extern std::atomic<int> iter;

// asynchronous worker
void async(int thread_id, QVI qvi, Params* params) {
	
	while(iter < params->max_outer_iter){
		
		// asynchronous
		qvi.update(iter);
		iter++;
	}
	return;
}
#endif