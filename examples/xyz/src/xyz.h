#ifndef _H_XYZ_H
#define _H_XYZ_H

#include "hiro.h"

using namespace hiro;

class XYZ : public HIRO
{
   public:
      // default constructor
      XYZ();

      // default destructor
      virtual ~XYZ();

      /// the virtual functions for the inner minimisation problem of the hard instance generator
      // function to solve an inner integer program of a robust problem
      virtual const HIROsolution solve_ip(int numelements, int numscenarios,
            std::vector< std::vector<double> > scenariocosts);

      /// Statistic information
      // sets the number of nodes processed in the last call to solve_ip
      void set_num_nodes(int _nodes);

      // returns the number of nodes processed in the last call to solve_ip
      int get_num_nodes();

   private:
      // solving statistics
      int nodes;
};
#endif
