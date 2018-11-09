#ifndef _H_SELECT_H
#define _H_SELECT_H

#include <vector>
#include "ilcplex/ilocplex.h"


struct Solution
{
	std::vector<double> x;
	double ub;
	int nodes;
};

class Select
{
	public:
		void generate_rand(int _n, int _p, int _N);
		void generate_normal(int _n, int _p, int _N);
		
		Solution solve_ip();
		Solution solve_regret();
		
		void generate_hard(int _n, int _p, int _N, bool solvecompact = true);
		void generate_hard_lazy(int _n, int _p, int _N);
		void generate_hard_alternateheuristic(int _n, int _p, int _N);
		void generate_hard_LDR(int _n, int _p, int _N);
		
		void generate_hard_alt(int _n, int _p, int _N);
		
		void generate_midpoint(int _n, int _p, int _N);
		void generate_midpointlp(int _n, int _p, int _N);
		
		void set_budget(int _scenbudget);
		
		//void generate_hard_heu(int _n, int _p, int _N);

		std::vector<std::vector<double> > get_c();
		void gen_U();
		void set_problem(int _n, int _p, int _N, std::vector<std::vector<double> > _c);
		
		void set_timelimit(double _timelimit);
		
		void print();

	private:
		int n,p;
		int N;
		std::vector<std::vector<double> > c;
		std::vector<std::vector<double> > nomc;
		
		std::vector<std::vector<double> > x;
		
		//bounds on c
		std::vector<std::vector<double> > cl;
		std::vector<std::vector<double> > cu;
		//constriants of the form <= 1
		//std::vector<std::vector<double> > constraints;
		
		double solve_master(bool relax=false);
		double solve_master_decomp(double lowerbound, bool relax=false);
      double solve_decomp_sub();
		double solve_master_alt();
		double solve_master_alternateheuristic();
		double solve_midlp_it();

		double build_decomp_sub(const IloEnv env, IloModel& model, IloCplex& cplex, IloObjective& objective,
         std::vector<std::vector<IloNumVar> >& cplexd, std::vector<IloNumVar>& cplexlambda, bool relax);
      double solve_decomp_sub(IloEnv& env, IloModel& model, IloCplex& cplex, IloObjective& objective,
         std::vector<std::vector<IloNumVar> > cplexd, std::vector<IloNumVar> cplexlambda, int j, double costdual,
         double convexdual, std::vector<std::vector<double> > scenduals, std::vector<double> exclude, double gap,
         bool enumerate, bool& add);
      double execute_column_generation(IloEnv& masterenv, IloModel& master, IloCplex& cpxmaster, IloEnv& env,
         IloModel& model, IloCplex& cplex, IloObjective& objective, std::vector<std::vector<IloNumVar> > cplexd,
         std::vector<IloNumVar>& cplexlambda, std::vector<std::vector<IloNumVar> >& cplexalpha,
         std::vector<IloRange> costcons, std::vector<IloRange> convexcons,
         std::vector<std::vector<std::vector<IloRange> > > scencons,
         std::vector<std::vector<std::vector<double> > >& columns, std::vector<int>& ncolumns, double lowerbound);

		int scenbudget;
};

#endif
