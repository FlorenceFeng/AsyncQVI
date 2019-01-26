#ifndef ORACLE_H
#define ORACLE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>
#include "util.h"
#define DIMWIND 8
using namespace std;
class Sailing{
    
	private:
		int x;
		int y;
		int wind;
		int DIMX;
		int DIMY;
		int GOALX;
		int GOALY;
		float wind_transition[DIMWIND][DIMWIND] = {
			{0.3, 0.2, 0.1, 0.04, 0.02, 0.04, 0.1, 0.2},
			{0.2, 0.3, 0.2, 0.1, 0.04, 0.02, 0.04, 0.1},
			{0.1, 0.2, 0.3, 0.2, 0.1, 0.04, 0.02, 0.04},
			{0.04, 0.1, 0.2, 0.3, 0.2, 0.1, 0.04, 0.02},
			{0.02, 0.04, 0.1, 0.2, 0.3, 0.2, 0.1, 0.04},
			{0.04, 0.02, 0.04, 0.1, 0.2, 0.3, 0.2, 0.1},
			{0.1, 0.04, 0.02, 0.04, 0.1, 0.2, 0.3, 0.2},
			{0.2, 0.1, 0.04, 0.02, 0.04, 0.1, 0.2, 0.3}
		};
	
	public:
		void setValues(Params* params){
			DIMX = (int)sqrt(params->len_state/DIMWIND);
			DIMY = DIMX;
			GOALX = DIMX-1;
			GOALY = DIMY-1;
		}
		
		void indexToState(int index){
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
			// noise
			/*double prob = randdouble(0,1);
			if(prob < 0.05){
				x = max(0, min(x + (int)normaldouble(0.,10.), DIMX-1));
				y = max(0, min(y + (int)normaldouble(0.,10.), DIMY-1));
				//x = randint(0, DIMX-1);
				//y = randint(0, DIMY-1);
			}*/	
		}
		
		double reward(int a){
			if( x == GOALX && y == GOALY){
				return 1.;
			}
			else if(x==0 & y==0)
				return 0.;
			else{	
				int d = abs(a - wind);
				d = d < 8 - d ? d : 8 - d;
				return d * 0.05;
			}
		}
		
		void windTransition(){
			double prob = randdouble(0, 1);
			double start = 0;
			for(int nwind = 0; nwind < DIMWIND; nwind++){
				start += wind_transition[wind][nwind];
				if(start > prob){
					wind = nwind;
					break;
				}
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

void test_sailing(Sailing& s, std::vector<int>* pi, Params* params){
	
	double start_time = get_wall_time();
	s.setValues(params);
	double total_reward = 0.;
	int flag = 0;
	int isflag = 0;
	for (int episode = 0; episode < params->test_max_episode; episode++){
		int i = randint(0,params->len_state-1);
		int j = 0;
		double r = 0;
		isflag = 0;
		for (int step = 0; step < params->test_max_step; step++){
			s.SO(i,(*pi)[i],j,r);
			total_reward += pow(params->gamma,step)*r;
			if(r == 1 && !isflag){
				flag += 1;
				isflag = 1;
				//break;
			}
			i = j;
			
		}
	}
	params->test_time += get_wall_time() - start_time;
	total_reward /= params->test_max_episode;
	cout<<get_wall_time()-params->test_time-params->time<<' '<<total_reward<<' '<<flag<<endl;
	
}
#endif