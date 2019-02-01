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
   // NOTE: This is required to generate hard instances
   sel.set_problem(type, n, N);
	
	sel.set_timelimit(atof(argv[5]));
	sel.set_budget(atoi(argv[6]));
   sel.set_num_select(p);
	
	double start = clock();
   bool success = sel.generate_hard_instance();

   // if the generation of hard instances fails, then the program terminates
   if (!success)
      return 0;
		
	cout<<"ALGTIME;"<<(clock()-start)/CLOCKS_PER_SEC<<"\n";
	
	//vector<vector<double> > c = sel.get_c();
	//for (int i=0; i<N; ++i)
		//for (int k=0; k<n; ++k)
			//cout<<"c["<<i<<"]["<<k<<"] = "<<c[i][k]<<"\n";
		
	//sel.print();
		
	start = clock();
	HIROsolution sol = sel.solve_ip();
	double soltime = (clock()-start)/CLOCKS_PER_SEC;
	cout<<"TIME;"<<soltime<<"\n";
	cout<<"NODES;"<<sol.get_nodes()<<"\n";
	cout<<"OBJ;"<<sol.get_upper_bound()<<"\n";
	
	
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
	
	/*
	//write to file
	vector<vector<double> > c = sel.get_c();
	ofstream out("training-40-40.dat", std::ios_base::app);
	//for (int i=0; i<N; ++i)
		//for (int j=0; j<n; ++j)
			//out<<c[i][j]<<";";
			
			
	//calculate statistics
	vector<double> rav(N,0);
	vector<double> rstd(N,0);
	for (int i=0; i<N; ++i)
	{
		for (int j=0; j<n; ++j)
			rav[i] += c[i][j];
		rav[i] = rav[i] / N;
		for (int j=0; j<n; ++j)
			rstd[i] += pow(c[i][j] - rav[i],2);
		rstd[i] = sqrt(rstd[i]/N);
	}
	
	vector<double> cav(n,0);
	vector<double> cstd(n,0);
	for (int j=0; j<n; ++j)
	{
		for (int i=0; i<N; ++i)
			cav[j] += c[i][j];
		cav[j] = cav[j] / N;
		for (int i=0; i<N; ++i)
			cstd[j] += pow(c[i][j] - cav[j],2);
		cstd[j] = sqrt(cstd[j]/N);
	}	
			
	for (int i=0; i<N; ++i)
		out<<rav[i]<<";";
	for (int i=0; i<N; ++i)
		out<<rstd[i]<<";";
	for (int j=0; j<n; ++j)
		out<<cav[j]<<";";
	for (int j=0; j<n; ++j)
		out<<cstd[j]<<";";		
			
// 	out<<soltime<<"\n";
	out<<sol.nodes<<"\n";
	out.close();
	*/
	
	
	return 0;
}
