#ifndef ASYNC_H_
#define ASYNC_H_

#include <cstdatomic>
#include <iostream>
#include <thread>
#include "util.h"
#include "qvi.h"
using namespace std;

extern std::atomic<int> iter;

// asynchronous worker
void async(int thread_id, QVI qvi, Params* params) {
	
	while(iter < params->max_outer_iter){
		
		// asynchronous
		bcd.update(iter);
		iter++;
	}
	return;
}
#endif