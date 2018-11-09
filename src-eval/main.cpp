#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "ilcplex/ilocplex.h"

ILOSTLBEGIN

using namespace std;

int main (int argc, char* argv[])
{
	
	//read file
	ifstream in(argv[1]);
	string line;
	
	getline(in,line);
	int n = atoi(line.c_str());
	size_t pos = line.find(";");
	line=line.substr(pos+1);
	
	int p = atoi(line.c_str());
	pos = line.find(";");
	line=line.substr(pos+1);
	
	int N = atoi(line.c_str());
	
	vector<vector<double> > c(N);
	for (int i=0; i<N; ++i)
		c[i].resize(n);
	
	int id = 0;
	while (!in.eof())
	{
		getline(in,line);
		
		// Ignore Comment and Empty Lines
		if (line[0]=='#' || line=="")
			continue;

		for (int j=0; j<n; ++j)
		{
			c[id][j] = atof(line.c_str());
			pos = line.find(";");
			line=line.substr(pos+1);
		}

		++id;
	}
	
	//solve problem
	IloEnv env;
	IloModel model(env);
			
	vector<IloNumVar> cplexx(n);
	for (int i=0; i<n; ++i)
		cplexx[i] = IloNumVar(env, 0, 1, ILOBOOL);
	
	IloNumVar cplexz(env,0,IloInfinity,ILOFLOAT);
	
	for (int i=0; i<N; ++i)
	{
		IloExpr con(env);
		for (int j=0; j<n; ++j)
			con += c[i][j] * cplexx[j];
		model.add(cplexz >= con); 
	}
	
	{
		IloExpr con(env);
		for (int j=0; j<n; ++j)
			con += cplexx[j];
		model.add(con == p);
	}
	
	model.add(IloMinimize(env, cplexz));
		
	IloCplex cplex(model);
	
	cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);
	cplex.setParam(IloCplex::PreInd,0);	
	
	double dstart = cplex.getDetTime();	
	double tstart = clock();
	
	bool result = cplex.solve();
	
	double dend = cplex.getDetTime();
	double tend = clock();
	
	
	//cout<<"DET TIME = "<<cplex.getDetTime()<<"\n";
	cout<<argv[1]<<";"<<(tend-tstart)/CLOCKS_PER_SEC<<";"<<dend-dstart<<";"<<cplex.getNnodes()<<";"<<cplex.getNiterations()<<"\n";
	//cout<<(clock()-tstart)/CLOCKS_PER_SEC<<"\n";
	//cout<<cplex.getNnodes()<<"\n";

	env.end();


	
}
