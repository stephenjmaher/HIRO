#ifndef _H_TSP_H
#define _H_TSP_H

#include "hiro.h"

using namespace hiro;

class TSP : public HIRO
{
   public:
      // default constructor
      TSP();

      // default destructor
      virtual ~TSP();

      /// the virtual functions for the inner minimisation problem of the hard instance generator
      // function to solve an inner integer program of a robust problem
      virtual const HIROsolution solve_ip();

      // function to solve the regret problem of the min-max regret problem
      // TODO: check the comment
      virtual const HIROsolution solve_regret();
};
#endif
