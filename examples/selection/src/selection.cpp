#include "selection.h"
#include <algorithm>

using namespace std;
using namespace hiro;

// default constructor
Selection::Selection()
   : HIRO()
{
}

Selection::~Selection()
{
}

const HIROsolution Selection::solve_ip()
{
   HIROsolution sol;

   // getting the member variables that define the problem
   int n = this->get_num_items();
   int N = this->get_num_scenarios();
   vector< vector<double> > c = this->get_c();

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
   cplex.setParam(IloCplex::PreInd, 0);

   bool result = cplex.solve();

   //TODO: Need to adds setsolution functions for nodes, ub and x

   sol.set_nodes(cplex.getNnodes());
   sol.set_upper_bound(cplex.getObjValue());

   vector<double> x(n, 0);
   for (int j=0; j<n; ++j)
      x[j] = cplex.getValue(cplexx[j]);
   sol.set_solution(x);

   env.end();

   return sol;
}

// sets the number of items to select
void Selection::set_num_select(int _p)
{
   assert(_p >= 0);
   p = _p;
}

// returns the number of items to select
int Selection::get_num_select()
{
   return p;
}
