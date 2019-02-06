#include "selection.h"

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
	srand(atoi(argv[7]));

   int type = atoi(argv[4]);
   int n = atoi(argv[1]);
   int N = atoi(argv[3]);
   int p = atoi(argv[2]);


	Selection sel;
   // setting the basic problem information
   // NOTE: This step is necessary to execute the program
   // ALTERNATIVELY: you can individually set the algorithm type, number of cost coefficients and number of scenarios.
   sel.set_problem(type, n, N);

   // setting additional parameters for the instance generation algorithm
	sel.set_timelimit(atof(argv[5]));
	sel.set_budget(atoi(argv[6]));
   sel.set_num_select(p);

	double start = clock();
   bool success = sel.generate_hard_instance();

   // if the generation of hard instances fails, then the program terminates
   if (!success)
      return 0;

   // printing statistics
	cout<<"ALGTIME;"<<(clock()-start)/CLOCKS_PER_SEC<<"\n";

	start = clock();
	HIROsolution sol = sel.solve_ip(sel.get_num_elements(), sel.get_num_scenarios(), sel.get_c());
	double soltime = (clock()-start)/CLOCKS_PER_SEC;
	cout<<"TIME;"<<soltime<<"\n";
	cout<<"NODES;"<<sol.get_nodes()<<"\n";
	cout<<"OBJ;"<<sol.get_upper_bound()<<"\n";

   // output the resulting instance to a file
	vector<vector<double> > c = sel.get_c();
	stringstream ss;
	ss<<"instance";
	for (int i=0; i<7; ++i)
		ss<<"-"<<argv[i+1];
	ss<<".dat";

	ofstream out(ss.str().c_str());
	out<<n<<";"<<p<<";"<<N<<"\n";
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<n; ++j)
			out<<c[i][j]<<";";
		out<<"\n";
	}
	out.close();

	return 0;
}
