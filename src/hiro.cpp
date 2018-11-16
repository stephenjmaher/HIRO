#include "select.h"
#include <algorithm>
#include <random>
#include <set>

ILOSTLBEGIN

#define NRCONS 100

using namespace std;


double timelimit = 30;

int __global_K;
int __global_curK;
vector<vector<double> > __globalc;
double __globalub;

ILOLAZYCONSTRAINTCALLBACK7(ScenarioLazyCallback, vector<vector<IloNumVar> >, cplexc,  vector<vector<IloNumVar> >, cplexlambda,  vector<vector<vector<IloNumVar> > >, cplexd, IloNumVar, cplext, int, _n, int, _p, int, _N)
{

	cout<<"**Calling lazy constraints**\n"<<flush;

	if (__global_curK == __global_K)
	{
		cout<<"**Max number of solutions reached.**\n"<<flush;
		return;
	}

   //find wc
   cout<<"**Building x**\n"<<flush;
   vector<vector<double> > cutc(_N);
   for (int i=0; i<_N; ++i)
   {
	   cutc[i].resize(_n);
	   for (int k=0; k<_n; ++k)
			cutc[i][k] = getValue(cplexc[i][k]);
   }


	cout<<"**Solving sub**\n"<<flush;
	Select sel;
	sel.set_problem(_n,_p,_N,cutc);

	double start = clock();
	Solution sol = sel.solve_ip();
	double end = clock();

	cout<<"**CURTIME;"<<(end-start)/CLOCKS_PER_SEC<<"\n";

   if (sol.ub > __globalub - 0.01)
   {
	   __globalub = sol.ub;
	   for (int i=0; i<_N; ++i)
		   for (int k=0; k<_n; ++k)
				__globalc[i][k] = cutc[i][k];
		cout<<"**NEW BEST VALUE "<<__globalub<<"**\n"<<flush;
   }


   cout<<"**Adding x NR "<<__global_curK<<"**\n"<<flush;
   if (getObjValue() > sol.ub + 0.01)
   {
	   //add constraints
	   IloEnv env = getEnv();

		{
			IloExpr con(env);
			for (int i=0; i<_N; ++i)
				for (int k=0; k<_n; ++k)
					con += sol.x[k] * cplexd[i][__global_curK][k];
			add(cplext <= con);
		}

		{
			IloExpr con(env);
			for (int i=0; i<_N; ++i)
				con += cplexlambda[__global_curK][i];
			add(con == 1);
		}

		++__global_curK;
   }


   return;

}





void Select::set_problem(int _n, int _p, int _N, std::vector<std::vector<double> > _c)
{
	n = _n;
	p = _p;
	N = _N;
	c = _c;
}






void Select::gen_U()
{
	c.resize(N);
	nomc.resize(N);
	for (int i=0; i<N; ++i)
	{
		c[i].resize(n,-1);
		nomc[i].resize(n,-1);
	}

	// find random lower and upper bounds
	cl.resize(N);
	cu.resize(N);

	for (int i=0; i<N; ++i)
	{
		cl[i].resize(n);
		cu[i].resize(n);

		for (int k=0; k<n; ++k)
		{
			c[i][k] = rand()%101;
			nomc[i][k] = c[i][k];

			cl[i][k] = c[i][k]-scenbudget;
			if (cl[i][k] < 0)
				cl[i][k] = 0;

			cu[i][k] = c[i][k]+scenbudget;
			if (cu[i][k] > 100)
				cu[i][k] = 100;
		}
	}

	// find random constraints
	//vector<int> inds(n);
	//for (int k=0; k<n; ++k)
		//inds[k] = k;

	//for (int i=0; i<NRCONS; ++i)
	//{
		//random_shuffle(inds.begin(), inds.end());

		//vector<double> con(n,0);
		//for (int k=0; k<n; ++k)
			//con[inds[k]] = 0.1*(rand()%1001)/1000.0;
		//constraints.push_back(con);
	//}

	//for (int i=0; i<1; ++i)
	//{
		////random_shuffle(inds.begin(), inds.end());

		//vector<double> con(n,0);
		//for (int k=0; k<n; ++k)
			//con[k] = 2.0 /n;
			////con[inds[k]] = (2.0 * ( 1 + (rand()%201)/1000.0 - .1) ) /n;
		//constraints.push_back(con);
	//}

}







void Select::generate_hard_lazy(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;

	gen_U();

	__globalc.resize(N);
	for (int i=0; i<N; ++i)
	{
		__globalc[i].resize(n);
		for (int k=0; k<n; ++k)
			__globalc[i][k] = c[i][k];
	}

	Solution startsol = solve_ip();
	x.push_back(startsol.x);
	__globalub = startsol.ub;

	//max lazy constraints
	__global_K = 10*N;
	__global_curK = 1;

	IloEnv env;
	IloModel model(env);

	vector<vector<IloNumVar> > cplexc(N);
	for (int i=0; i<N; ++i)
	{
		cplexc[i].resize(n);
		for (int k=0; k<n; ++k)
			cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
	}

	vector<vector<IloNumVar> > cplexlambda(__global_K);
	for (int j=0; j<__global_K; ++j)
	{
		cplexlambda[j].resize(N);
		for (int i=0; i<N; ++i)
			cplexlambda[j][i] = IloNumVar(env,0,1,ILOBOOL);
	}

	vector<vector<vector<IloNumVar> > > cplexd(N);
	for (int i=0; i<N; ++i)
	{
		cplexd[i].resize(__global_K);
		for (int j=0; j<__global_K; ++j)
		{
			cplexd[i][j].resize(n);
			for (int k=0; k<n; ++k)
				cplexd[i][j][k] = IloNumVar(env,0,cu[i][k],ILOFLOAT);
		}
	}

	IloNumVar cplext(env,0,IloInfinity,ILOFLOAT);

	for (int j=0; j<__global_curK; ++j)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
				con += x[j][k] * cplexd[i][j][k];
		model.add(cplext <= con);
	}

	for (int j=0; j<__global_curK; ++j)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			con += cplexlambda[j][i];
		model.add(con == 1);
	}

	//use global K here
	for (int i=0; i<N; ++i)
		for (int j=0; j<__global_K; ++j)
			for (int k=0; k<n; ++k)
			{
				model.add(cplexd[i][j][k] <= cplexc[i][k]);
				model.add(cplexd[i][j][k] <= cu[i][k]*cplexlambda[j][i]);
			}


	//for (int l=0; l<constraints.size(); ++l)
		for (int i=0; i<N; ++i)
		{
			double csum = 0;
			IloExpr con(env);
			for (int k=0; k<n; ++k)
			{
				csum += nomc[i][k];
				con += cplexc[i][k];
			}
			model.add(con == csum);
		}

	model.add(IloMaximize(env, cplext));

	IloCplex cplex(model);

	//warmstart
	if (c[0][0] > -0.5)
	{
		IloNumVarArray startVar(env);
		IloNumArray startVal(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
			{
				startVar.add(cplexc[i][k]);
				startVal.add(c[i][k]);
			}
		cplex.addMIPStart(startVar, startVal);
		startVal.end();
		startVar.end();
	}

	//cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);
	cplex.setParam(IloCplex::TiLim, timelimit);

	cplex.use(ScenarioLazyCallback(env,cplexc,cplexlambda,cplexd,cplext,n,p,N));

	bool result = cplex.solve();

	for (int i=0; i<N; ++i)
		for (int k=0; k<n; ++k)
			c[i][k] = __globalc[i][k];

	env.end();

}








void  Select::generate_hard(int _n, int _p, int _N, bool solvecompact)
{
	n = _n;
	p = _p;
	N = _N;

	gen_U();

	Solution startsol = solve_ip();
	x.push_back(startsol.x);

	double bestobj = startsol.ub;
	vector<vector<double> > bestc = c;

	double cstart = clock();

	//solve master
	double obj = solve_master();
   double lowerbound = obj;

	//solve sub
	double start = clock();
	Solution sol = solve_ip();
	double end = clock();
	if (sol.ub > bestobj - 0.01)
	{
		bestobj = sol.ub;
		bestc = c;
	}


	int itnr = 1;
	cout<<itnr<<";"<<obj<<";"<<bestobj<<";"<<(end-start)/CLOCKS_PER_SEC<<"\n"<<flush;

	while(obj > bestobj + 0.01 && (clock()-cstart)/CLOCKS_PER_SEC < timelimit)
	{
		x.push_back(sol.x);

      double masterstart = clock();
      if (solvecompact)
         obj = solve_master();
      else
         obj = solve_master_decomp(lowerbound);
      double masterend = clock();

		start = clock();
		sol = solve_ip();
		end = clock();

		if (sol.ub > bestobj - 0.01)
		{
			cout<<"*";
			bestobj = sol.ub;
			bestc = c;
		}

		++itnr;
		cout<<itnr<<";"<<obj<<";"<<bestobj<<";"<<sol.ub<<";";
      cout<<(end-start)/CLOCKS_PER_SEC<<";"<<(masterend-masterstart)/CLOCKS_PER_SEC<<"\n"<<flush;
	}

//	c = bestc;
	
}


void Select::generate_hard_LDR(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;
	
	gen_U();
	
	vector<double> lambda0(N);
	vector<vector<double> > lambda(N);
	for (int i=0; i<N; ++i)
		lambda[i].resize(n);
	
	bool repeat = true;
	double lastval = 0;
	double curt = clock();
	
	do
	{
		double val = 0;
		
		//solve lambda-step
		{
			IloEnv env;
			IloModel model(env);
			
			
			//variables
			//cout<<"Variables, first model.\n"<<flush;
			vector<vector<IloNumVar> > cplexxi(n);
			vector<vector<IloNumVar> > cplexzeta(n);
			for (int k=0; k<n; ++k)
			{
				cplexxi[k].resize(n);
				cplexzeta[k].resize(n);
				for (int l=0; l<n; ++l)
				{
					cplexxi[k][l] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
					cplexzeta[k][l] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
				}
			}
			
			vector<IloNumVar> cplextheta(n);
			for (int k=0; k<n; ++k)
				cplextheta[k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
				
			IloNumVar cplexeta(env, -IloInfinity, IloInfinity, ILOFLOAT);
			IloNumVar cplexpi(env, -IloInfinity, IloInfinity, ILOFLOAT);
			
			vector<IloNumVar> cplexrho(n);
			for (int k=0; k<n; ++k)
				cplexrho[k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
				
			vector<IloNumVar> cplexalpha(N);
			for (int i=0; i<N; ++i)
				cplexalpha[i] = IloNumVar(env, -IloInfinity, IloInfinity, ILOFLOAT);
			
			vector<vector<IloNumVar> > cplexbeta(N);
			for (int i=0; i<N; ++i)
			{
				cplexbeta[i].resize(n);
				for (int k=0; k<n; ++k)
					cplexbeta[i][k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
			}
			
			vector<IloNumVar> cplexlambda0(N);
			for (int i=0; i<N; ++i)
				cplexlambda0[i] = IloNumVar(env, -IloInfinity, IloInfinity, ILOFLOAT);
			vector<vector<IloNumVar> > cplexlambda(N);
			for (int i=0; i<N; ++i)
			{
				cplexlambda[i].resize(n);
				for (int k=0; k<n; ++k)
					cplexlambda[i][k] = IloNumVar(env, -IloInfinity, IloInfinity, ILOFLOAT);
			}
			
			//constraints
			//cout<<"Constraints, first model.\n"<<flush;
			for (int k=0; k<n; ++k)
			{
				IloExpr con(env);
				for (int l=0; l<n; ++l)
					con += cplexxi[k][l] + cplexxi[l][k] - cplexzeta[k][l] - cplexzeta[l][k];
				con += cplexeta;
				con -= cplextheta[k];
				for (int i=0; i<N; ++i)
					con -= cplexlambda0[i] * c[i][k];
				model.add(con <= 0);
			}
			
			for (int k=0; k<n; ++k)
				for (int l=0; l<n; ++l)
				{
					IloExpr con(env);
					for (int i=0; i<N; ++i)
						con += cplexlambda[i][l] * c[i][k];
					model.add(-2*cplexxi[k][l] + cplexzeta[k][l] <= con);
				}
			
			{
				IloExpr con(env);
				for (int k=0; k<n; ++k)
					con += cplexrho[k];
				for (int i=0; i<N; ++i)
					con += cplexlambda0[i];
				model.add( p*cplexpi + con <= 1);
			}	
			
			for (int k=0; k<n; ++k)
			{
				IloExpr con(env);
				for (int i=0; i<N; ++i)
					con += cplexlambda[i][k];
				model.add(cplexpi + cplexrho[k] >= con);
			}
			
			for (int i=0; i<N; ++i)
			{
				IloExpr con(env);
				for (int k=0; k<n; ++k)
					con += cplexbeta[i][k];
				model.add( cplexlambda0[i] + p*cplexalpha[i] - con >= 0);
			}
			
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
				{
					model.add( cplexalpha[i] - cplexbeta[i][k] <= cplexlambda[i][k]);
				}
				
			//objective & solution
			IloExpr obj(env);
			for (int k=0; k<n; ++k)
				for (int l=0; l<n; ++l)
					obj -= cplexzeta[k][l];
			obj += p*cplexeta;
			for (int k=0; k<n; ++k)
				obj -= cplextheta[k];
				
			model.add(IloMaximize(env, obj));
						
			IloCplex cplex(model);	
						
			cplex.setOut(env.getNullStream());
			cplex.setParam(IloCplex::Threads, 1);
			
			bool result = cplex.solve();
			
			cout<<cplex.getStatus()<<"\n";
			
			for (int i=0; i<N; ++i)
			{
				lambda0[i] = cplex.getValue(cplexlambda0[i]);
				//cout<<"lambda0["<<i<<"] = "<<lambda0[i]<<"\n";
				for (int k=0; k<n; ++k)
				{
					lambda[i][k] = cplex.getValue(cplexlambda[i][k]);
					//cout<<"lambda["<<i<<"]["<<k<<"] = "<<lambda[i][k]<<"\n";
				}
			}
			
			//for (int k=0; k<n; ++k)
				//for (int l=0; l<n; ++l)
					//cout<<"zeta["<<k<<"]["<<l<<"] = "<<cplex.getValue(cplexzeta[k][l])<<"\n";
			//cout<<"eta = "<<cplex.getValue(cplexeta)<<"\n";
			//for (int k=0; k<n; ++k)
				//cout<<"theta["<<k<<"] = "<<cplex.getValue(cplextheta[k])<<"\n";
			
			val = cplex.getObjValue();
			cout<<"val is "<<val<<"\n"<<flush;
			//double tval = 0;
			//for (int k=0; k<n; ++k)
				//for (int l=0; l<n; ++l)
					//tval -= cplex.getValue(cplexzeta[k][l]);
			//tval += p*cplex.getValue(cplexeta);
			//for (int k=0; k<n; ++k)
				//tval -= cplex.getValue(cplextheta[k]);
			//cout<<"tval is "<<tval<<"\n";
			
			env.end();
			//exit(0);
		}
	
		//solve c-step
		{
			IloEnv env;
			IloModel model(env);
			
			
			//variables
			vector<vector<IloNumVar> > cplexxi(n);
			vector<vector<IloNumVar> > cplexzeta(n);
			for (int k=0; k<n; ++k)
			{
				cplexxi[k].resize(n);
				cplexzeta[k].resize(n);
				for (int l=0; l<n; ++l)
				{
					cplexxi[k][l] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
					cplexzeta[k][l] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
				}
			}
			
			vector<IloNumVar> cplextheta(n);
			for (int k=0; k<n; ++k)
				cplextheta[k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
				
			IloNumVar cplexeta(env, -IloInfinity, IloInfinity, ILOFLOAT);
			
			vector<vector<IloNumVar> > cplexc(N);
			for (int i=0; i<N; ++i)
			{
				cplexc[i].resize(n);
				for (int k=0; k<n; ++k)
					cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
			}
			
			//constraints
			for (int k=0; k<n; ++k)
			{
				IloExpr con(env);
				for (int l=0; l<n; ++l)
					con += cplexxi[k][l] + cplexxi[l][k] - cplexzeta[k][l] - cplexzeta[l][k];
				con += cplexeta;
				con -= cplextheta[k];
				for (int i=0; i<N; ++i)
					con -= lambda0[i]*cplexc[i][k];
				model.add(con <= 0);
			}

			for (int k=0; k<n; ++k)
				for (int l=0; l<n; ++l)
				{
					IloExpr con(env);
					for (int i=0; i<N; ++i)
						con += lambda[i][l] * cplexc[i][k];
					model.add(-2*cplexxi[k][l] + cplexzeta[k][l] <= con);
				}
				
			for (int i=0; i<N; ++i)
			{
				double csum = 0;
				IloExpr con(env);
				for (int k=0; k<n; ++k)
				{
					csum += nomc[i][k];
					con += cplexc[i][k];
				}
				model.add(con == csum);
			}
				
			//objective & solution
			IloExpr obj(env);
			for (int k=0; k<n; ++k)
				for (int l=0; l<n; ++l)
					obj -= cplexzeta[k][l];
			obj += p*cplexeta;
			for (int k=0; k<n; ++k)
				obj -= cplextheta[k];
				
			model.add(IloMaximize(env, obj));
						
			IloCplex cplex(model);	
			
			cplex.setOut(env.getNullStream());
			cplex.setParam(IloCplex::Threads, 1);
			
			bool result = cplex.solve();
			
			
			cout<<cplex.getStatus()<<"\n";
			
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					c[i][k] = cplex.getValue(cplexc[i][k]);
			
			val = cplex.getObjValue();
			
			env.end();
		}
		
		cout<<"val is "<<val<<"\n"<<flush;
		
		if (val > lastval + 0.001)
			lastval = val;
		else
			repeat = false;
			
	}while(repeat && (clock()-curt)/CLOCKS_PER_SEC < timelimit);	
}


void  Select::generate_hard_alt(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;

	gen_U();
	vector<vector<double> > bestc = c;

	Solution startsol = solve_ip();
	x.push_back(startsol.x);

	double cstart = clock();

	int itnr = 0;
	bool repeat = true;
	while(repeat && (clock()-cstart)/CLOCKS_PER_SEC < timelimit)
	{
		double obj = solve_master_alt();

		double start = clock();
		Solution sol = solve_ip();
		double end = clock();

		double nomval = 0;
		double altval = 0;
		for (int i=0; i<N; ++i)
		{
			double inomval = 0;
			double ialtval = 0;
			for (int k=0; k<n; ++k)
			{
				inomval += x[0][k]*c[i][k];
				ialtval += sol.x[k]*c[i][k];
			}
			if (inomval > nomval)
				nomval = inomval;
			if (ialtval > altval)
				altval = ialtval;
		}
		cout<<nomval<<";"<<altval<<"\n";
		if (nomval > altval + 0.01)
			x.push_back(sol.x);
		else
			repeat = false;

		bestc = c;

		++itnr;
		cout<<itnr<<";"<<obj<<";"<<(end-start)/CLOCKS_PER_SEC<<"\n"<<flush;
	}

	c = bestc;

}




//void Select::generate_hard_heu(int _n, int _p, int _N)
//{
	//n = _n;
	//p = _p;
	//N = _N;

	//c.resize(N);
	//for (int i=0; i<N; ++i)
		//c[i].resize(n,-1);

	//gen_U();

	//// find random constraints
	//vector<int> inds(n);
	//for (int k=0; k<n; ++k)
		//inds[k] = k;


	////add start sols
	//for (int j=0; j<100; ++j)
	//{
		//vector<double> tempx(n,0);
		//random_shuffle(inds.begin(), inds.end());
		//for (int k=0; k<p; ++k)
			//tempx[inds[k]] = 1;
		//x.push_back(tempx);
	//}

	////solve master
	//double obj = solve_master(false);
//}

void Select::generate_hard_alternateheuristic(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;
	
	gen_U();
	
	Solution sol = solve_ip();
	x.push_back(sol.x);
	set<vector<int> > pool;
	vector<int> curx(n,0);
	for (int k=0; k<n; ++k)
		if (sol.x[k] > 0.5)
			curx[k] = 1;
	pool.insert(curx);
	
	double curt = clock();
	
	int it=0;
	cout<<"Main iteration "<<it<<"\n"<<flush;
	double val = solve_master_alternateheuristic();	
	bool repeat = true;
	do
	{
		sol = solve_ip();
		vector<int> tempx(n,0);
		for (int k=0; k<n; ++k)
			if (sol.x[k] > 0.5)
			{
				tempx[k] = 1;
				//cout<<"Pack item "<<k<<"\n";
			}
		
		if (pool.count(tempx) > 0.5)
			repeat = false;
		else
		{
			pool.insert(tempx);
			++it;
			cout<<"Main iteration "<<it<<"\n"<<flush;
			x.push_back(sol.x);
			val = solve_master_alternateheuristic();	
		}
	}
	while(repeat && (clock()-curt)/CLOCKS_PER_SEC < timelimit);
}

double Select::solve_master_alternateheuristic()
{
	int K = x.size();
	bool repeat = true;
	
	//double obj = 9999999;
	//for (int j=0; j<K; ++j)
	//{
		//double maxval = 0;
		//for (int i=0; i<N; ++i)
		//{
			//double tval = 0;
			//for (int k=0; k<n; ++k)
				//tval += x[j][k]*c[i][k];
			//if (tval > maxval)
				//maxval = tval;
		//}
		//if (maxval < obj)
			//obj = maxval;
	//}
	
	int it=0;
	
	double obj = 0;
	do
	{
		cout<<"Sub iteration "<<it<<"\n"<<flush;
		++it;
		
		//fix assignment
		vector<vector<int> > ass(N);
		for (int j=0; j<K; ++j)
		{
			//find largest scenario and assign
			double maxval = 0;
			int maxind = 0;
			for (int i=0; i<N; ++i)
			{
				double tval = 0;
				for (int k=0; k<n; ++k)
					tval += x[j][k]*c[i][k];
				if (tval > maxval)
				{
					maxval = tval;
					maxind = i;
				}
			}
			ass[maxind].push_back(j);
		}
		
		//for (int i=0; i<N; ++i)
			//for (int j=0; j<ass[i].size(); ++j)
				//cout<<"assign solution "<<ass[i][j]<<" to scenario "<<i<<"\n";
		
		//max each
		for (int i=0; i<N; ++i)
			if (ass[i].size() > 0.5)
			{	
				IloEnv env; 
				IloModel model(env);
				
				vector<IloNumVar> cplexc(n);
				for (int k=0; k<n; ++k)
					cplexc[k] = IloNumVar(env, cl[i][k],cu[i][k],ILOFLOAT);
				IloNumVar cplext(env, 0, IloInfinity, ILOFLOAT);
				
				for (int j=0; j<ass[i].size(); ++j)
				{
					IloExpr con(env);
					for (int k=0; k<n; ++k)
						con += x[ass[i][j]][k] * cplexc[k];
					model.add( cplext <= con);
				}
				
				{
					double csum = 0;
					IloExpr con(env);
					for (int k=0; k<n; ++k)
					{
						csum += nomc[i][k];
						con += cplexc[k];
					}
					model.add(con == csum);
				}
				
				model.add(IloMaximize(env,cplext));
				
				IloCplex cplex(model);
								
				cplex.setOut(env.getNullStream());
				cplex.setParam(IloCplex::Threads, 1);
				cplex.setParam(IloCplex::PreInd, 0);
				
				bool result = cplex.solve();
				
				for (int k=0; k<n; ++k)
					c[i][k] = cplex.getValue(cplexc[k]);
				
				env.end();				
			}
		
		double cur = 9999999;
		for (int j=0; j<K; ++j)
		{
			double maxval = 0;
			for (int i=0; i<N; ++i)
			{
				double tval = 0;
				for (int k=0; k<n; ++k)
					tval += x[j][k]*c[i][k];
				if (tval > maxval)
					maxval = tval;
			}
			if (maxval < cur)
				cur = maxval;
		}
		//cout<<cur<<";"<<obj<<"\n";
		if (cur < obj + 0.01)
			repeat = false;
		obj = cur;		
	}
	while(repeat);
	
	return obj;
}


double Select::solve_master(bool relax)
{
	int K = x.size();

	IloEnv env;
	IloModel model(env);

	vector<vector<IloNumVar> > cplexc(N);
	for (int i=0; i<N; ++i)
	{
		cplexc[i].resize(n);
		for (int k=0; k<n; ++k)
			cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
	}

	vector<vector<IloNumVar> > cplexlambda(K);
	for (int j=0; j<K; ++j)
	{
		cplexlambda[j].resize(N);
		if (relax)
			for (int i=0; i<N; ++i)
				cplexlambda[j][i] = IloNumVar(env,0,1,ILOFLOAT);
		else
			for (int i=0; i<N; ++i)
				cplexlambda[j][i] = IloNumVar(env,0,1,ILOBOOL);
	}

	vector<vector<vector<IloNumVar> > > cplexd(N);
	for (int i=0; i<N; ++i)
	{
		cplexd[i].resize(K);
		for (int j=0; j<K; ++j)
		{
			cplexd[i][j].resize(n);
			for (int k=0; k<n; ++k)
				cplexd[i][j][k] = IloNumVar(env,0,cu[i][k],ILOFLOAT);
		}
	}

	IloNumVar cplext(env,0,IloInfinity,ILOFLOAT);

	for (int j=0; j<K; ++j)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
				con += x[j][k] * cplexd[i][j][k];
		model.add(cplext <= con);
	}

	for (int j=0; j<K; ++j)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			con += cplexlambda[j][i];
		model.add(con == 1);
	}

	for (int i=0; i<N; ++i)
		for (int j=0; j<K; ++j)
			for (int k=0; k<n; ++k)
			{
				model.add(cplexd[i][j][k] <= cplexc[i][k]);
				model.add(cplexd[i][j][k] <= cu[i][k]*cplexlambda[j][i]);
			}


	//for (int l=0; l<constraints.size(); ++l)
		//for (int i=0; i<N; ++i)
		//{
			//IloExpr con(env);
			//for (int k=0; k<n; ++k)
				//con += constraints[l][k] * (cplexc[i][k] - cl[i][k]) / (cu[i][k] - cl[i][k]);
			//model.add(con <= 1);
		//}
	for (int i=0; i<N; ++i)
	{
		double csum = 0;
		IloExpr con(env);
		for (int k=0; k<n; ++k)
		{
			csum += nomc[i][k];
			con += cplexc[i][k];
		}
		model.add(con == csum);
	}
		
	model.add(IloMaximize(env, cplext));

	IloCplex cplex(model);

	//warmstart
	if (c[0][0] > -0.5)
	{
		IloNumVarArray startVar(env);
		IloNumArray startVal(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
			{
				startVar.add(cplexc[i][k]);
				startVal.add(c[i][k]);
			}
		cplex.addMIPStart(startVar, startVal);
		startVal.end();
		startVar.end();
	}

	cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);

	bool result = cplex.solve();

	for (int i=0; i<N; ++i)
		for (int k=0; k<n; ++k)
			c[i][k] = cplex.getValue(cplexc[i][k]);

	double obj = cplex.getObjValue();

	env.end();

	return obj;
}


double Select::execute_column_generation(IloEnv& masterenv, IloModel& master, IloCplex& cpxmaster, IloEnv& env,
   IloModel& model, IloCplex& cplex, IloObjective& objective, vector<vector<IloNumVar> > cplexd,
   vector<IloNumVar>& cplexlambda, vector<vector<IloNumVar> >& cplexalpha, vector<IloRange> costcons,
   vector<IloRange> convexcons, vector<vector<vector<IloRange> > > scencons, vector<vector<vector<double> > >& columns,
   vector<int>& ncolumns, double lowerbound)
{
	int K = x.size();

   bool added;
   bool enumerate = false;
   double lpobj;
   double gap = 0.0;
   int subprobidx = 0;
   vector<bool> addvar(K, false);
   vector<double> addvarcost(K, 0);
   vector<int> addedcolumns(K, 0);

   vector<double> exclude(N*n, -1);

   do
   {
#ifdef ALG_DEBUG
      cout<<"==================="<<endl;
#endif
      added = false;
      cpxmaster = IloCplex(master);
      //cpxmaster.exportModel("master.lp");

      cpxmaster.setParam(IloCplex::Param::Preprocessing::Presolve, 0);
      cpxmaster.setOut(masterenv.getNullStream());
      cpxmaster.setParam(IloCplex::Threads, 1);

      bool result = cpxmaster.solve();
      assert(result == 1);

      lpobj = cpxmaster.getObjValue();

      int subprobcnt = 0;
      int firstsubprob = subprobidx;
      while (subprobcnt<K && !added)
      {
         bool varadded;
         int j = subprobidx;

         /* if we are enumerating columns, then we need to reset the exclude function */
         if (enumerate)
         {
            exclude.clear();
            exclude.resize(N*n, -1);
         }

         vector<vector<double> > scenduals(N, vector<double>(n, 0));
         for (int i=0; i<N; ++i)
            for (int k=0; k<n; ++k)
               scenduals[i][k] = cpxmaster.getDual(scencons[j][i][k]);

         do
         {
            varadded = false;
            double colcost = solve_decomp_sub(env, model, cplex, objective, cplexd, cplexlambda, j,
               cpxmaster.getDual(costcons[j]), cpxmaster.getDual(convexcons[j]), scenduals, exclude, gap, enumerate,
               varadded);

            if (varadded)
            {
               int costvectorindex = -1;
#ifdef ALG_DEBUG
               cout<<j<<" varadded"<<endl;
#endif
               cplexalpha[j].push_back(IloNumVar(masterenv,0,IloInfinity,ILOFLOAT));
               addvar[j] = true;
               addvarcost[j] = colcost;

               // storing the column
               vector<double> column;
               bool foundexclude = false;
               for (int i=0; i<N; ++i)
               {
                  for (int k=0; k<n; ++k)
                  {
                     column.push_back(cplex.getValue(cplexd[i][k]));

                     if (!foundexclude && column.back() > 0)
                     {
                        costvectorindex = i*N + k;
                        foundexclude = true;
                     }
                  }
               }

               columns[j].push_back(column);
               ncolumns[j]++;
               addedcolumns[j]++;

               /* if columns are being enumerated, then the exclude array must be updated */
               if (enumerate)
               {
                  if (exclude[costvectorindex] >= 0)
                  {
                     int i = int(costvectorindex/N);
                     int k = costvectorindex % N;
                     exclude[costvectorindex] = cu[i][k] - 1e-2;
                  }
                  else
                     exclude[costvectorindex] -= 1e-2;
               }
            }

         }
         while( varadded && enumerate );

         added = added || varadded;

         subprobidx++;
         subprobcnt++;
         if (subprobidx == K)
            subprobidx = 0;
      }

      int addvarcnt = 0;
      subprobidx = firstsubprob;
      while (addvarcnt < subprobcnt)
      {
         int j = subprobidx;
#ifdef ALG_DEBUG
         cout<<j<<" addvar - "<<addvar[j]<<" addvarcost - "<<addvarcost[j]<<endl;
#endif

         while( addedcolumns[j] > 0 )
         {
            costcons[j].setLinearCoef(cplexalpha[j][ncolumns[j] - addedcolumns[j]], -addvarcost[j]);
            convexcons[j].setLinearCoef(cplexalpha[j][ncolumns[j] - addedcolumns[j]], 1.0);

            for (int i=0; i<N; ++i)
               for (int k=0; k<n; ++k)
                  scencons[j][i][k].setLinearCoef(cplexalpha[j][ncolumns[j] - addedcolumns[j]],
                     columns[j][ncolumns[j] - addedcolumns[j]][i*n + k]);

            addedcolumns[j]--;
         }

         // resetting the addvar and addvarcost arrays
         addvar[j] = false;
         addvarcost[j] = 0;
         addedcolumns[j] = 0;
         //columns[j].clear();

         subprobidx++;
         addvarcnt++;
         if (subprobidx == K)
            subprobidx = 0;
      }

      /* when no columns are added, then the lp has been solved. It is then necessary to enumerate columns. */
      if (!added && !enumerate && false)
      {
         enumerate = true;
         gap = lowerbound - lpobj;

         added = true;
      }
   }
   while (added);

   return lpobj;
}


double Select::solve_master_decomp(double lowerbound, bool relax)
{
	int K = x.size();

   IloEnv masterenv;
   IloModel master(masterenv);
   IloObjective masterobj(masterenv);
   IloCplex cpxmaster;

	IloEnv env;
	IloModel model(env);
   IloCplex cplex(env);
   IloObjective objective(env);
   vector<vector<IloNumVar> > cplexd;
   vector<IloNumVar> cplexlambda(N);

   vector<vector<IloNumVar> > cplexc(N);
   vector<vector<IloNumVar> > cplexalpha(K);
   vector<int> ncolumns(K, 0);
   vector<vector<vector<double> > > columns(K);
   vector<IloRange> costcons;
   vector<IloRange> convexcons;
   vector<vector<vector<IloRange> > > scencons(K);

   // building the subproblem
   build_decomp_sub(env, model, cplex, objective, cplexd, cplexlambda, relax);

   // adding the variables and constraints for the master problem
	for (int i=0; i<N; ++i)
	{
		cplexc[i].resize(n);
		for (int k=0; k<n; ++k)
			cplexc[i][k] = IloNumVar(masterenv,cl[i][k],cu[i][k],ILOFLOAT);
	}

   //for (int i=0; i<N; ++i)
   //{
      //double csum = 0;
      //IloExpr con(masterenv);
      //for (int k=0; k<n; ++k)
      //{
         //csum += nomc[i][k];
         //con += cplexc[i][k];
      //}
      //master.add(con == csum);
   //}

	IloNumVar cplext(masterenv,0,IloInfinity,ILOFLOAT);

	for (int j=0; j<K; ++j)
	{
      double colcost = 0;
      vector<double> column;
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
         {
            double coef = cu[i][k];
            if (i > 0)
               coef = 0.0;

				colcost += x[j][k] * coef;

            column.push_back(coef);
         }

		cplexalpha[j].push_back(IloNumVar(masterenv,0,IloInfinity,ILOFLOAT));
      columns[j].push_back(column);
      ncolumns[j]++;

      // adding the constraints
      costcons.push_back(IloRange(masterenv, cplext - colcost*cplexalpha[j][0], 0));
      convexcons.push_back(IloRange(masterenv, cplexalpha[j][0], 1));
      //convexcons[j].setBounds(1.0, 1.0);

      scencons[j].resize(N);
      for (int i=0; i<N; ++i)
      {
         for (int k=0; k<n; ++k)
         {
            double coef = cu[i][k];
            if (i > 0)
               coef = 0.0;

            scencons[j][i].push_back(IloRange(masterenv, cplexalpha[j][0]*coef - cplexc[i][k], 0));

            master.add(scencons[j][i][k]);
         }
      }

      master.add(costcons[j]);
      master.add(convexcons[j]);
	}

   // adding the objective function
   IloExpr obj(masterenv);
   obj += cplext;
   for (int i=0; i<N; ++i)
      for (int k=0; k<n; ++k)
         obj -= cplexc[i][k] * 1e-12;

   masterobj.setExpr(obj);
   masterobj.setSense(IloObjective::Maximize);

	master.add(masterobj);

	obj.end();

   double lpobj = execute_column_generation(masterenv, master, cpxmaster, env, model, cplex, objective, cplexd,
      cplexlambda, cplexalpha, costcons, convexcons, scencons, columns, ncolumns, lowerbound);

   //master.add(masterlp);

   for (int j=0; j<K; ++j)
      for (int p=0; p<ncolumns[j]; ++p )
         master.add(IloConversion(masterenv, cplexalpha[j][p], ILOINT));

   cpxmaster = IloCplex(master);
   cpxmaster.setOut(masterenv.getNullStream());
   cpxmaster.setParam(IloCplex::Threads, 1);


   //cpxmaster.exportModel("intmaster.lp");

   bool result = cpxmaster.solve();
   assert(result == 1);

   for (int i=0; i<N; ++i)
      for (int k=0; k<n; ++k)
         c[i][k] = cpxmaster.getValue(cplexc[i][k]);

	double ipobj = cpxmaster.getObjValue();

   cout<<"LP obj: "<<lpobj<<" IP obj: "<<ipobj<<endl;

	env.end();
   masterenv.end();

	return ipobj;
}


double Select::build_decomp_sub(const IloEnv env, IloModel& model, IloCplex& cplex, IloObjective& objective,
   vector<vector<IloNumVar> >& cplexd, vector<IloNumVar>& cplexlambda, bool relax)
{
   if (relax)
      for (int i=0; i<N; ++i)
         cplexlambda[i] = IloNumVar(env,0,1,ILOFLOAT);
   else
      for (int i=0; i<N; ++i)
         cplexlambda[i] = IloNumVar(env,0,1,ILOBOOL);

   vector<vector<IloNumVar> > cplexc(N);
   for (int i=0; i<N; ++i)
   {
      cplexc[i].resize(n);
      for (int k=0; k<n; ++k)
         cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
   }

   cplexd.resize(N);
	for (int i=0; i<N; ++i)
	{
      cplexd[i].resize(n);
      for (int k=0; k<n; ++k)
         cplexd[i][k] = IloNumVar(env,0,cu[i][k],ILOFLOAT);
	}

   for (int i=0; i<N; ++i)
   {
      double csum = 0;
      IloExpr con(env);
      for (int k=0; k<n; ++k)
      {
         csum += nomc[i][k];
         con += cplexc[i][k];
      }
      model.add(con == csum);
   }

   IloExpr con(env);
   for (int i=0; i<N; ++i)
      con += cplexlambda[i];
   model.add(con == 1);

	for (int i=0; i<N; ++i)
      for (int k=0; k<n; ++k)
      {
         model.add(cplexd[i][k] <= cplexc[i][k]);
         model.add(cplexd[i][k] <= cu[i][k]*cplexlambda[i]);
      }

   // adding the objective function
   IloExpr obj(env);
   for (int i=0; i<N; ++i)
      for (int k=0; k<n; ++k)
         obj += x[0][k] * cplexd[i][k];

   objective.setExpr(obj);
   objective.setSense(IloObjective::Maximize);

	model.add(objective);

   obj.end();

   cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);
}


double Select::solve_decomp_sub(IloEnv& env, IloModel& model, IloCplex& cplex, IloObjective& objective,
   vector<vector<IloNumVar> > cplexd, vector<IloNumVar> cplexlambda, int j, double costdual, double convexdual,
   vector<vector<double> > scenduals, vector<double> exclude, double gap, bool enumerate, bool& add)
{

   // adding the objective function
   IloExpr obj(env);
   for (int i=0; i<N; ++i)
      for (int k=0; k<n; ++k)
         obj += x[j][k] * cplexd[i][k] * costdual - cplexd[i][k] * scenduals[i][k];

   objective.setExpr(obj);
   objective.setSense(IloObjective::Maximize);

	obj.end();

   /* is column enumeration is performed, then we need to adjust the bounds of the lambda variables */
   if (enumerate)
   {
      for (int i=0; i<N; ++i)
         for (int k=0; k<n; ++k)
            if (exclude[i*N + k] >= 0)
               cplexd[i][k].setUB(exclude[i*N + k]);
   }


	cplex.extract(model);
   //cplex.exportModel("subprob.lp");

   bool status = cplex.solve();
   assert(status == 1);

   double redcost = cplex.getObjValue() - convexdual;

   // if the reduced cost is positive, then we add the variable to the master problem.
   double columncost = 0;
   if (redcost > gap + 1e-04)
   {
      add = true;

      for (int i=0; i<N; ++i)
         for (int k=0; k<n; ++k)
            columncost += x[j][k] * cplex.getValue(cplexd[i][k]);
   }

   if (columncost == 0)
      add = false;

   return columncost;
}




double Select::solve_master_alt()
{
	int K = x.size();

	IloEnv env;
	IloModel model(env);

	vector<vector<IloNumVar> > cplexc(N);
	for (int i=0; i<N; ++i)
	{
		cplexc[i].resize(n);
		for (int k=0; k<n; ++k)
			if (x[0][k] > 0.5)
				cplexc[i][k] = IloNumVar(env,nomc[i][k],cu[i][k],ILOFLOAT);
			else
				cplexc[i][k] = IloNumVar(env,cl[i][k],nomc[i][k],ILOFLOAT);
	}

	vector<vector<IloNumVar> > cplexlambda(K);
	for (int j=0; j<K; ++j)
	{
		cplexlambda[j].resize(N);
		for (int i=0; i<N; ++i)
			cplexlambda[j][i] = IloNumVar(env,0,1,ILOBOOL);
	}

	vector<vector<vector<IloNumVar> > > cplexd(N);
	for (int i=0; i<N; ++i)
	{
		cplexd[i].resize(K);
		for (int j=0; j<K; ++j)
		{
			cplexd[i][j].resize(n);
			for (int k=0; k<n; ++k)
				cplexd[i][j][k] = IloNumVar(env,0,cu[i][k],ILOFLOAT);
		}
	}

	IloNumVar cplext(env,0,IloInfinity,ILOFLOAT);

	for (int i=0; i<N; ++i)
	{
		IloExpr con(env);
		for (int k=0; k<n; ++k)
			con += cplexc[i][k]*x[0][k];
		for (int l=0; l<N; ++l)
			for (int k=0; k<n; ++k)
				con -= (1.0/N)*cplexc[l][k]*x[0][k];
		model.add(cplext >= con);
		model.add(con >= -cplext);
	}


	for (int l=0; l<N; ++l)
		for (int j=0; j<K; ++j)
		{
			IloExpr con(env);
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					con += x[j][k] * cplexd[i][j][k];
			IloExpr lhs(env);
			for (int k=0; k<n; ++k)
				lhs += cplexc[l][k]*x[0][k];

			model.add(lhs <= con);
		}

	for (int j=0; j<K; ++j)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			con += cplexlambda[j][i];
		model.add(con == 1);
	}

	for (int i=0; i<N; ++i)
		for (int j=0; j<K; ++j)
			for (int k=0; k<n; ++k)
			{
				model.add(cplexd[i][j][k] <= cplexc[i][k]);
				model.add(cplexd[i][j][k] <= cu[i][k]*cplexlambda[j][i]);
			}


	//for (int l=0; l<constraints.size(); ++l)
		//for (int i=0; i<N; ++i)
		//{
			//IloExpr con(env);
			//for (int k=0; k<n; ++k)
				//con += constraints[l][k] * (cplexc[i][k] - cl[i][k]) / (cu[i][k] - cl[i][k]);
			//model.add(con <= 1);
		//}
	for (int i=0; i<N; ++i)
	{
		double csum = 0;
		IloExpr con(env);
		for (int k=0; k<n; ++k)
		{
			csum += nomc[i][k];
			con += cplexc[i][k];
		}
		model.add(con == csum);
	}

	model.add(IloMinimize(env, cplext));

	IloCplex cplex(model);

	//warmstart
	if (c[0][0] > -0.5)
	{
		IloNumVarArray startVar(env);
		IloNumArray startVal(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
			{
				startVar.add(cplexc[i][k]);
				startVal.add(c[i][k]);
			}
		cplex.addMIPStart(startVar, startVal);
		startVal.end();
		startVar.end();
	}

	cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);

	bool result = cplex.solve();

	for (int i=0; i<N; ++i)
		for (int k=0; k<n; ++k)
			c[i][k] = cplex.getValue(cplexc[i][k]);

	double obj = cplex.getObjValue();

	env.end();

	return obj;
}










void Select::generate_rand(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;

	gen_U();
};

void Select::generate_normal(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(50.0,20.0);

	c.resize(N);
	nomc.resize(N);
	for (int i=0; i<N; ++i)
	{
		c[i].resize(n);
		for (int k=0; k<n; ++k)
			c[i][k] = distribution(generator);
	}

}

Solution Select::solve_ip()
{
	Solution sol;

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

	sol.nodes = cplex.getNnodes();
	sol.ub = cplex.getObjValue();
	sol.x.resize(n);
	for (int j=0; j<n; ++j)
		sol.x[j] = cplex.getValue(cplexx[j]);

	env.end();

	return sol;
}


Solution Select::solve_regret()
{
	vector<double> opt(N,0);
	for (int i=0; i<N; ++i)
	{
		vector<double> tcost = c[i];
		sort(tcost.begin(),tcost.end());
		for (int k=0; k<p; ++k)
			opt[i] += tcost[k];
	}

	Solution sol;

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
		model.add(cplexz >= con - opt[i]);
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

	bool result = cplex.solve();

	sol.ub = cplex.getObjValue();
	sol.x.resize(n);
	for (int j=0; j<n; ++j)
		sol.x[j] = cplex.getValue(cplexx[j]);

	env.end();

	return sol;
}


vector<vector<double> > Select::get_c()
{
	return c;
}

void Select::set_timelimit(double _timelimit)
{
	timelimit = _timelimit;
}




void  Select::generate_midpoint(int _n, int _p, int _N)
{
	n = _n;
	p = _p;
	N = _N;

	gen_U();

	IloEnv env;
	IloModel model(env);


	//variables
	vector<IloNumVar> cplexx(n);
	for (int k=0; k<n; ++k)
		cplexx[k] = IloNumVar(env, 0, 1, ILOBOOL);

	vector<IloNumVar> cplexlambda(N);
	for (int i=0; i<N; ++i)
		cplexlambda[i] = IloNumVar(env, 0, 1, ILOBOOL);

	vector<vector<IloNumVar> > cplexc(N);
	for (int i=0; i<N; ++i)
	{
		cplexc[i].resize(n);
		for (int k=0; k<n; ++k)
			cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
	}

	vector<IloNumVar> cplext(N);
	for (int i=0; i<N; ++i)
		cplext[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);

	IloNumVar cplexalpha (env,0,IloInfinity,ILOFLOAT);

	vector<IloNumVar> cplexbeta(n);
	for (int k=0; k<n; ++k)
		cplexbeta[k] = IloNumVar(env, 0, IloInfinity,ILOFLOAT);

	vector<vector<IloNumVar> > cplexr(N);
	for (int i=0; i<N; ++i)
	{
		cplexr[i].resize(n);
		for (int k=0; k<n; ++k)
			cplexr[i][k] = IloNumVar(env,0,cu[i][k],ILOFLOAT);
	}

	vector<IloNumVar> cplexq(N);
	for (int i=0; i<N; ++i)
		cplexq[i] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);


	//constraints
	for (int i=0; i<N; ++i)
	{
		IloExpr con(env);
		for (int k=0; k<n; ++k)
			con += cplexr[i][k];
		model.add(cplext[i] == con);
	}

	for (int i=0; i<N; ++i)
		model.add(cplexq[i] <= cplext[i]);

	for (int i=0; i<N; ++i)
	{
		IloExpr con(env);
		double val = 0;
		for (int k=0; k<n; ++k)
			val += cu[i][k];
		model.add(cplexq[i] <= val*cplexlambda[i]);
	}

	for (int i=0; i<N; ++i)
		for (int k=0; k<n; ++k)
		{
			model.add(cplexr[i][k] <= cplexc[i][k]);
			model.add(cplexr[i][k] <= cu[i][k] * cplexx[k]);
			model.add(cplexr[i][k] >= cplexc[i][k] + cu[i][k]*(cplexx[k] - 1));
		}

	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			con += cplexlambda[i];
		model.add(con == 1);
	}

	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
				con += cplexr[i][k];
		for (int k=0; k<n; ++k)
			con += cplexbeta[k];
		model.add(con == p*cplexalpha);
	}

	{
		IloExpr con(env);
		for (int k=0; k<n; ++k)
			con += cplexx[k];
		model.add(con == p);
	}

	for (int k=0; k<n; ++k)
	{
		IloExpr con(env);
		for (int i=0; i<N; ++i)
			con += cplexc[i][k];
		model.add(cplexalpha - cplexbeta[k] <= con);
	}

	//for (int l=0; l<constraints.size(); ++l)
		//for (int i=0; i<N; ++i)
		//{
			//IloExpr con(env);
			//for (int k=0; k<n; ++k)
				//con += constraints[l][k] * (cplexc[i][k] - cl[i][k]) / (cu[i][k] - cl[i][k]);
			//model.add(con <= 1);
		//}

	for (int i=0; i<N; ++i)
	{
		double csum = 0;
		IloExpr con(env);
		for (int k=0; k<n; ++k)
		{
			csum += nomc[i][k];
			con += cplexc[i][k];
		}
		model.add(con == csum);
	}


	IloExpr obj(env);
	for (int i=0; i<N; ++i)
		obj += cplexq[i];

	model.add(IloMaximize(env, obj));

	IloCplex cplex(model);

	//warmstart
	if (c[0][0] > -0.5)
	{
		IloNumVarArray startVar(env);
		IloNumArray startVal(env);
		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
			{
				startVar.add(cplexc[i][k]);
				startVal.add(c[i][k]);
			}
		cplex.addMIPStart(startVar, startVal);
		startVal.end();
		startVar.end();
	}

	//cplex.setOut(env.getNullStream());
	cplex.setParam(IloCplex::Threads, 1);
	cplex.setParam(IloCplex::TiLim, timelimit);

	bool result = cplex.solve();

	for (int i=0; i<N; ++i)
		for (int k=0; k<n; ++k)
			c[i][k] = cplex.getValue(cplexc[i][k]);

	env.end();

}


void Select::generate_midpointlp(int _n, int _p, int _N)
{

	n = _n;
	p = _p;
	N = _N;

	gen_U();

	double succ = solve_midlp_it();
	double prev = succ + 1 ;

	while( succ < prev - 0.00001)
	{
		cout<<"It.\n"<<flush;
		prev = succ;
		succ = solve_midlp_it();
	}

}


double Select::solve_midlp_it()
{
	//opt for alpha
	vector<vector<double> > alpha;
	{
		IloEnv env;
		IloModel model(env);

		vector<vector<IloNumVar> > cplexalpha(N);
		for (int i=0; i<N; ++i)
		{
			cplexalpha[i].resize(n);
			for (int k=0; k<n; ++k)
				cplexalpha[i][k] = IloNumVar(env, 0, IloInfinity, ILOFLOAT);
		}

		IloNumVar cplexbeta(env, 0, IloInfinity, ILOFLOAT);

		{
			IloExpr con(env);
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					con += c[i][k]*cplexalpha[i][k];
			model.add(con >= 1);
		}

		for (int l=0; l<N; ++l)
		{
			IloExpr con(env);
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					con += c[l][k]*cplexalpha[i][k];
			model.add(cplexbeta >= con);
		}

		model.add(IloMinimize(env, cplexbeta));

		IloCplex cplex(model);

		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::Threads, 1);

		bool result = cplex.solve();

		alpha.resize(N);
		for (int i=0; i<N; ++i)
		{
			alpha[i].resize(n);
			for (int k=0; k<n; ++k)
				alpha[i][k] = cplex.getValue(cplexalpha[i][k]);
		}

		cout<<"Sub1: "<<cplex.getObjValue()<<"\n";

		env.end();
	}

	double obj = 0;
	//opt for c
	{
		IloEnv env;
		IloModel model(env);

		vector<vector<IloNumVar> > cplexc(N);
		for (int i=0; i<N; ++i)
		{
			cplexc[i].resize(n);
			for (int k=0; k<n; ++k)
				cplexc[i][k] = IloNumVar(env,cl[i][k],cu[i][k],ILOFLOAT);
		}

		IloNumVar cplexbeta(env, 0, IloInfinity, ILOFLOAT);

		{
			IloExpr con(env);
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					con += cplexc[i][k]*alpha[i][k];
			model.add(con >= 1);
		}

		for (int l=0; l<N; ++l)
		{
			IloExpr con(env);
			for (int i=0; i<N; ++i)
				for (int k=0; k<n; ++k)
					con += cplexc[l][k]*alpha[i][k];
			model.add(cplexbeta >= con);
		}

		//for (int l=0; l<constraints.size(); ++l)
			//for (int i=0; i<N; ++i)
			//{
				//IloExpr con(env);
				//for (int k=0; k<n; ++k)
					//con += constraints[l][k] * (cplexc[i][k] - cl[i][k]) / (cu[i][k] - cl[i][k]);
				//model.add(con <= 1);
			//}

		for (int i=0; i<N; ++i)
		{
			double csum = 0;
			IloExpr con(env);
			for (int k=0; k<n; ++k)
			{
				csum += nomc[i][k];
				con += cplexc[i][k];
			}
			model.add(con == csum);
		}


		model.add(IloMinimize(env, cplexbeta));

		IloCplex cplex(model);

		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::Threads, 1);

		bool result = cplex.solve();

		for (int i=0; i<N; ++i)
			for (int k=0; k<n; ++k)
				c[i][k] = cplex.getValue(cplexc[i][k]);

		obj = cplex.getObjValue();
		cout<<"Sub2: "<<cplex.getObjValue()<<"\n";

		env.end();
	}


	return obj;
}


void Select::set_budget(int _scenbudget)
{
	scenbudget = _scenbudget;
}

void Select::print()
{
	for (int i=0; i<N; ++i)
	{
		for (int k=0; k<n; ++k)
			cout<<c[i][k]<<";";
		cout<<"\n";
	}
}
