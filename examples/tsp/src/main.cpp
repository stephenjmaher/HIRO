#include "tsp.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>

using namespace std;
using namespace hiro;

int main (int argc, char* argv[])
{
   // Input parameters:
   // 1: number of edges in the graph
   // 2: number of scenarios
   // 3: the hard instance algorithm type
   // 4: time limit
   // 5: budget
   // 6: random seed

   int n = atoi(argv[1]);
   int N = atoi(argv[2]);
   int type = atoi(argv[3]);

   // setting the basic problem information
   // NOTE: This is required to generate hard instances
	TSP tsp;

   tsp.set_problem(type, n, N);

   // setting the time limit and budget
	tsp.set_timelimit(atof(argv[4]));
	tsp.set_budget(atoi(argv[5]));

   // setting the random seed for the instance generator
   if (argc >= 7)
   	srand(atoi(argv[6]));

	double start = clock();
   bool success = tsp.generate_hard_instance();

   // if the generation of hard instances fails, then the program terminates
   if (!success)
      return 0;

	cout<<"ALGTIME;"<<(clock()-start)/CLOCKS_PER_SEC<<"\n";

	start = clock();
	HIROsolution sol = tsp.solve_ip(tsp.get_num_elements(), tsp.get_num_scenarios(), tsp.get_c());
	double soltime = (clock()-start)/CLOCKS_PER_SEC;
	cout<<"TIME;"<<soltime<<"\n";
	cout<<"NODES;"<<sol.get_nodes()<<"\n";
	cout<<"OBJ;"<<sol.get_upper_bound()<<"\n";

	vector<vector<double> > c = tsp.get_c();
	stringstream ss;
	ss<<"instance";
	for (int i=0; i<6; ++i)
   {
		ss<<"-"<<argv[i+1];
   }
	ss<<".dat";

	ofstream out(ss.str().c_str());
	out<<n<<";"<<N<<"\n";
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<n; ++j)
			out<<c[i][j]<<";";
		out<<"\n";
	}
	out.close();

	return 0;
}
