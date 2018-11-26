#ifndef ASYNC_H_
#define ASYNC_H_

#include <atomic>
#include <iostream>
#include <thread>
#include "qlearning.h"
using namespace std;

extern std::atomic<int> iter; // global iteration counter

// asynchronous worker
void async(int thread_id, Qlearning qlearning, Params* params) {
	
	while(iter < params->max_iter){
		qlearning.update(iter);
		iter++;
	}
	return;
}
#endif
