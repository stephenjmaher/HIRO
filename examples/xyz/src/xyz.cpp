#include "xyz.h"
#include <algorithm>
#include <set>

using namespace std;
using namespace hiro;

// default constructor
XYZ::XYZ()
   : HIRO()
{
}

XYZ::~XYZ()
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

const HIROsolution XYZ::solve_ip(int numelements, int numscenarios, vector< vector<double> > scenariocosts)
{
   // NOTE: This function must be implemented
   //HIROsolution sol;

	//return sol;
}

// sets the number of nodes processed in the last call to solve_ip
void XYZ::set_num_nodes(int _nodes)
{
   assert(_nodes >= 0);
   nodes = _nodes;
}

// returns the number of nodes processed in the last call to solve_ip
int XYZ::get_num_nodes()
{
   return nodes;
}
