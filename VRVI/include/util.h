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
	int max_outer_loop;
	int max_inner_loop;
	int sample_num;
	double gamma = 0.99;
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
		else if (std::string(argv[i - 1]) == "-max_outer_loop") {
			para->max_outer_loop = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-max_inner_loop") {
			para->max_inner_loop = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-sample_num") {
			para->sample_num = atoi(argv[i]);
		}
		else if (std::string(argv[i - 1]) == "-gamma") {
			para->gamma = atof(argv[i]);
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
		
#endif#include