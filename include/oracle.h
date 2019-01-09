#ifndef ORACLE_H
#define ORACLE_H

#include <iostream>
#include <iomanip>
#include <string>
#include "util.h"

#define DIMX 100
#define DIMY 100
#define DIMWIND 8
#define GOALX 50
#define GOALY 50

class Sailing{
    
	private:
		int x;
		int y;
		int wind;
		bool flag = 0;
		float wind_transition[DIMWIND][DIMWIND] = {
			{0.4, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3},
			{0.4, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0},
			{0.0, 0.4, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0},
			{0.0, 0.0, 0.4, 0.3, 0.3, 0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0, 0.4, 0.2, 0.4, 0.0, 0.0},
			{0.0, 0.0, 0.0, 0.0, 0.3, 0.3, 0.4, 0.0},
			{0.0, 0.0, 0.0, 0.0, 0.0, 0.3, 0.3, 0.4},
			{0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, 0.3}
		};
	
	public:
		void indexToState(int index){
			// index = wind_ * (dim_x * dim_y) + x * dim_y + y
			wind = index / (DIMX * DIMY);
			x = (index - DIMX * DIMY * wind) / DIMY;
			y = index - DIMX * DIMY * wind - DIMY * x;
		}
	
		int stateToIndex(){
			return wind * DIMX * DIMY + x * DIMY + y;
		}
		
		std::pair<int, int> direction(int a){
			switch( a ) {
				case 0: return std::make_pair(0, 1);
				case 1: return std::make_pair(1, 1);
				case 2: return std::make_pair(1, 0);
				case 3: return std::make_pair(1, -1);
				case 4: return std::make_pair(0, -1);
				case 5: return std::make_pair(-1, -1);
				case 6: return std::make_pair(-1, 0);
				case 7: return std::make_pair(-1, 1);
				default: return std::make_pair(-1, -1);
			}
		}
		
		void apply(int a){
			std::pair<int, int> dir = direction(a);
			x = max(0, min(x + dir.first, DIMX-1));
			y = max(0, min(y + dir.second, DIMY-1));
		}
		
		double reward(int a){
			if( x == GOALX && y == GOALY){
				flag = 1;
				return 1;
			}
			int d = abs(a - wind);
			d = d < 8 - d ? d : 8 - d;
			return d * 0.05;
		}
		
		void windTransition(){
			double prob = uniformRand();
			double start = 0;
			for(int nwind = 0; nwind < DIMWIND; nwind++){
				start += wind_transition[wind][nwind];
				if(start > prob)
					wind = nwind;
				break;
			}
		}
		
		void SO(int i, int a, int& j, double& r){
			indexToState(i);
			apply(a);
			r = reward(a);
			windTransition();
			j = stateToIndex();
		}
};

void test_sailing(std::vector<int>* pi){
	Sailing s;
	double total_reward = 0.;
	int max_episode = 100;
	int max_iter = 100;
	int flag = 0;
	for (int episode = 0; episode < max_episode; episode++){
		int i = 0;
		int j = 0;
		double r = 0;
		for (int iter = 0; iter < max_iter; iter++){
			s.SO(i,(*pi)[i],j,r);
			total_reward += pow(params.gamma,iter)*r;
			if(r == 1){
				flag += 1;
				break;
			}
			i = j;
		}
	}
	total_reward /= max_episode;
	cout<<"total_reward is "<<total_reward<<endl;
	cout<<"flag is "<<flag<<"/100"<<endl;
}
#endif