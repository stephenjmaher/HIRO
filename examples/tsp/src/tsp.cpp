#include "tsp.h"
#include <algorithm>
#include <set>

using namespace std;
using namespace hiro;

// default constructor
TSP::TSP()
   : HIRO()
{
}

TSP::~TSP()
{
}

ILOLAZYCONSTRAINTCALLBACK2(CycleLazyCallback, vector<vector<IloNumVar> >, cpx, int, nr_nodes)
{
   //cout<<"**Calling lazy constraints**\n"<<flush;

   IloEnv env = getEnv();

   vector<vector<bool> > sol(nr_nodes);
   for (int i=0; i<nr_nodes; ++i)
   {
	   sol[i].resize(nr_nodes,false);
		for (int j=0; j<nr_nodes; ++j)
			try
			{
				if (getValue(cpx[i][j])>0.5)
					sol[i][j] = true;
			}
			catch(IloException& e)
			{}
	}

	vector<vector<pair<int,int> > > paths;
	bool find_cycle;
	set<int> unreached;
	for (int i=0; i<nr_nodes; ++i)
		unreached.insert(i);

	while(unreached.size() > 0)
	{
		find_cycle = false;
		set<int> reached;
		vector<pair<int,int> > path;

		int current = *(unreached.begin());
		reached.insert(current);
		unreached.erase(current);

		do
		{
			int next;
			for (int i=0; i<nr_nodes; ++i)
				if (sol[current][i])
					next = i;
			if (reached.count(next) > 0)
			{
				path.push_back(pair<int,int>(current,next));
				find_cycle = true;
			}
			else
			{
				reached.insert(next);
				unreached.erase(next);
				path.push_back(pair<int,int>(current,next));
				current = next;
			}

		}while(!find_cycle);

		paths.push_back(path);
	}

	if (paths.size() > 1)
	{
		//cout<<paths.size()<<" cycles found!\n"<<flush;
		for (vector<vector<pair<int,int> > >::iterator p=paths.begin(); p!=paths.end(); ++p)
		{
			IloRange con(env,0,p->size() - 1);

			for (vector<pair<int,int> >::const_iterator it=p->begin(); it!=p->end(); ++it)
				con.setLinearCoef(cpx[it->first][it->second],1);

			add(con);
		}
	}

   return;
}

const HIROsolution TSP::solve_ip(int numelements, int numscenarios, vector< vector<double> > scenariocosts)
{
   HIROsolution sol;

   // getting the member variables that define the problem
   int n = numelements;
   int nr_nodes = sqrt(n);
   int K = numscenarios;
   vector< vector<double> > c = scenariocosts;

	//cout<<"Solving problem with "<<nr_nodes<<" cities.\n";

	//build mapping from matrix to vector
	vector<vector<int> > dmap(nr_nodes);
	for (int i=0; i<nr_nodes; ++i)
	{
		dmap[i].resize(nr_nodes);
		for (int j=0; j<nr_nodes; ++j)
			dmap[i][j] = i*nr_nodes + j;
	}

   IloEnv env;
   IloModel model(env);

	//variables
	vector<vector<IloNumVar> > cpx(nr_nodes);
	for (int i=0; i<nr_nodes; ++i)
	{
		cpx[i].resize(nr_nodes);
		for (int j=0; j<nr_nodes; ++j)
			cpx[i][j] = IloNumVar(env,0,1,ILOBOOL);
	}

	IloNumVar cpz(env,0,IloInfinity,ILOFLOAT);

	for (int k=0; k<K; ++k)
	{
		IloExpr con(env);
		for (int i=0; i<nr_nodes; ++i)
			for (int j=0; j<nr_nodes; ++j)
				con += c[k][dmap[i][j]] * cpx[i][j];
		model.add(cpz >= con);
	}

	//incon
	for (int i=0; i<nr_nodes; ++i)
	{
		IloRange con(env,1,1);

		for (int j=0; j<nr_nodes; ++j)
			if (i != j)
				con.setLinearCoef(cpx[j][i],1);

		model.add(con);
	}

	//outcon
	for (int i=0; i<nr_nodes; ++i)
	{
		IloRange con(env,1,1);

		for (int j=0; j<nr_nodes; ++j)
			if (i != j)
				con.setLinearCoef(cpx[i][j],1);

		model.add(con);
	}


	model.add(IloMinimize(env, cpz));

	IloCplex cplex(model);

	cplex.setOut(env.getNullStream());
   cplex.setParam(IloCplex::Threads, 1);
	cplex.use(CycleLazyCallback(env,cpx,nr_nodes));

	bool result = cplex.solve();

	sol.set_upper_bound(cplex.getObjValue());

	vector<double> x(n, 0);
	for (int i=0; i<nr_nodes; ++i)
		for (int j=0; j<nr_nodes; ++j)
			x[dmap[i][j]] = cplex.getValue(cpx[i][j]);
	sol.set_solution(x);

   // setting the solving statistics
   set_num_nodes(cplex.getNnodes());

	env.end();

	return sol;
}

// sets the number of nodes processed in the last call to solve_ip
void TSP::set_num_nodes(int _nodes)
{
   assert(_nodes >= 0);
   nodes = _nodes;
}

// returns the number of nodes processed in the last call to solve_ip
int TSP::get_num_nodes()
{
   return nodes;
}
