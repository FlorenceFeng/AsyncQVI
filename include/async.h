#ifndef ASYNC_H_
#define ASYNC_H_

#include <iostream>
#include <thread>
#include "algo.h"
using namespace std;

extern std::atomic<int> iter;

// asynchronous worker
template <typename T>
void async(int thread_id, T t, Params* params) {
	
	while(iter < params->max_outer_iter){
		
		// asynchronous
		t.update(iter);
		iter++;
	}
	return;
}
#endif