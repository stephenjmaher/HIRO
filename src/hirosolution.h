#ifndef _H_HIROSOLUTION_H
#define _H_HIROSOLUTION_H

#include <vector>

class HIROsolution
{
   // default constructor
   HIROsolution();

   // constructor with inputs
   HIROsolution(std::vector<double> x, double ub, int nodes);

   // assignment operator
   //HIROsolution& operator=(const HIROsolution& rhs);

   // copy constructor
   //HIROsolution(const HIROsolution& rhs);

   // destructor
   virtual ~HIROsolution();

   // sets the values of the solution x
   void set_solution(std:vector<double> x);

   // sets the upper bound
   void set_upper_bound(double ub);

   // sets the number of nodes processed
   void set_nodes(int nodes);

   // gets the values of the solution x
   std::vector<double> get_solution();

   // gets the upper bound
   double get_upper_bound();

   // gets the number of nodes processed
   int get_nodes();

   private:
      std::vector<double> _x;
      double _ub;
      int _nodes;
};

#endif
