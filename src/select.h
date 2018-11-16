#ifndef _H_SELECT_H
#define _H_SELECT_H

#include <vector>
#include "ilcplex/ilocplex.h"

enum HIRO_HardAlgType
{
   RANDOM      = 0,
   NORMAL      = 1
   ITERATIVE   = 2,
   LAZY        = 3,
   MIDPOINT    = 4,
   MIDPOINTLP  = 5,
   ALT         = 6,
   ALTERNATE   = 7,
   LDR         = 8,
   COLGEN      = 9
};
typedef enum HIRO_AlgType HIRO_ALGTYPE;

struct Solution
{
	std::vector<double> x;
	double ub;
	int nodes;
};

class Select
{
	public:
      /// the virtual functions for the inner minimisation problem of the hard instance generator
      // function to solve an inner integer program of a robust problem
      virtual Solution solve_ip() = 0;

      // function to solve the regret problem of the min-max regret problem
      // TODO: check the comment
		virtual Solution solve_regret() = 0;

      /// functions for generating random instances.
      // random instances sampled from a uniform distribution
		void generate_rand(int _n, int _p, int _N);

      // random instances sampled from a normal distribution
		void generate_normal(int _n, int _p, int _N);

      /// functions to generate the hard instances
      // hard instances generated by the exact iterative method or the relaxed column generation method
		void generate_hard(int _n, int _p, int _N, bool solvecompact = true);

      // ???
		void generate_hard_lazy(int _n, int _p, int _N);

      // hard instances generated by using the alternating heuristic method
		void generate_hard_alternateheuristic(int _n, int _p, int _N);

      // hard instances generated by using linear decision rules
		void generate_hard_LDR(int _n, int _p, int _N);

      // ???
		void generate_hard_alt(int _n, int _p, int _N);

      // hard instances generated using the midpoint method
		void generate_midpoint(int _n, int _p, int _N);

      // hard instances generated using the LP of the midpoint integer program
		void generate_midpointlp(int _n, int _p, int _N);

      /// sets the parameters for the problem
      // sets the scenario budget
		void set_budget(int _scenbudget);

      // sets the time limit for the experiments
		void set_timelimit(double _timelimit);

		std::vector<std::vector<double> > get_c();
		void gen_U();
		void set_problem(int _n, int _p, int _N, std::vector<std::vector<double> > _c);

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
