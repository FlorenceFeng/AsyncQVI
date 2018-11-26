#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <random>
using namespace std; 

struct Params{
	int len_state;
	int len_action;
	int max_episode;
	int max_iter;
	int total_num_threads;
	double alpha = 0.5;
	double gamma = 0.99;
	double epsilon= 0.2;
	int style;
};

void parse_input_argv(Params* para, int argc, char *argv[]){
	
	if (argc < 2) {
		cout << "Input number error: [0]" << endl;
		return;
	}

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] != '-') {
			break;
		}
		if (++i >= argc) {
			cout << "Input number error: [1]" << endl;
			return;
		}
		else if (std::string(argv[i - 1]) == "-len_state") {
			para->len_state = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-len_action") {
			para->len_action = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-max_episode") {
			para->max_episode = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-max_iter") {
			para->max_iter = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-nthreads") {
			para->total_num_threads = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-style") {
			para->style = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-gamma") {
			para->gamma = atof(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-alpha") {
			para->alpha = atof(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-epsilon") {
			para->epsilon = atof(argv[i]);
		}
		else {
			cout << "Input number error: [2]" << endl;
			return;
		}
	}
	return;
}

int randnum(int start, int end){// including start and end
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(start, end); // guaranteed unbiased
	return uni(rng);
}

//  Windows
#ifdef _WIN32
#include <Windows.h>
double get_wall_time(){
  LARGE_INTEGER time,freq;
  if (!QueryPerformanceFrequency(&freq)){
    //  Handle error
    return 0;
  }
  if (!QueryPerformanceCounter(&time)){
    //  Handle error
    return 0;
  }
  return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time(){
  FILETIME a,b,c,d;
  if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
    //  Returns total user time.
    //  Can be tweaked to include kernel times as well.
    return
        (double)(d.dwLowDateTime |
                 ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
  }else{
    //  Handle error
    return 0;
  }
}

//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,NULL)){
    //  Handle error
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
  return (double)clock() / CLOCKS_PER_SEC;
}
#endif
		
#endif