#ifndef _H_SELECTION_H
#define _H_SELECTION_H

#include "hiro.h"

using namespace hiro;

class Selection : public HIRO
{
   public:
      // default constructor
      Selection();

      // default destructor
      virtual ~Selection();

      /// the virtual functions for the inner minimisation problem of the hard instance generator
      // function to solve an inner integer program of a robust problem
      virtual const HIROsolution solve_ip(int numelements, int numscenarios,
         std::vector< std::vector<double> > scenariocosts);

      // sets the number of items to select
      void set_num_select(int _p);

      // returns the number of items to select
      int get_num_select();

      /// Statistic information
      // sets the number of nodes processed in the last call to solve_ip
      void set_num_nodes(int _nodes);

      // returns the number of nodes processed in the last call to solve_ip
      int get_num_nodes();

   private:
      // private variables for the selection problem
      int p;   // the number of items to select

      // solving statistics
      int nodes;
};
#endif
