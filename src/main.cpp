#include "select.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>

using namespace std;

int main (int argc, char* argv[])
{
	srand(atoi(argv[7]));
	
	int n = atoi(argv[1]);
	int p = atoi(argv[2]);
	int N = atoi(argv[3]);
	int para = atoi(argv[4]);	
	
	Select sel;
	sel.set_timelimit(atoi(argv[5]));
	sel.set_budget(atoi(argv[6]));
	
	double start = clock();
	if (para == 0)
		sel.generate_rand(n,p,N);
	else if (para == 1)
		sel.generate_hard(n,p,N);
	else if (para == 2)
		sel.generate_hard_lazy(n,p,N);
	else if (para == 3)
		sel.generate_midpoint(n,p,N);
	else if (para == 4)
		sel.generate_midpointlp(n,p,N);
	else if (para == 5)
		sel.generate_hard_alt(n,p,N);
	else if (para == 6)
		sel.generate_normal(n,p,N);	
	else if (para == 7)
		sel.generate_hard_alternateheuristic(n,p,N);			
	else if (para == 8)
		sel.generate_hard_LDR(n,p,N);
   else if (para == 9)
      sel.generate_hard(n,p,N,false);
		
	cout<<"ALGTIME;"<<(clock()-start)/CLOCKS_PER_SEC<<"\n";
	
	//vector<vector<double> > c = sel.get_c();
	//for (int i=0; i<N; ++i)
		//for (int k=0; k<n; ++k)
			//cout<<"c["<<i<<"]["<<k<<"] = "<<c[i][k]<<"\n";
		
	//sel.print();
		
	start = clock();
	Solution sol = sel.solve_ip();
	double soltime = (clock()-start)/CLOCKS_PER_SEC;
	cout<<"TIME;"<<soltime<<"\n";
	cout<<"NODES;"<<sol.nodes<<"\n";
	cout<<"OBJ;"<<sol.ub<<"\n";
	
	
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
