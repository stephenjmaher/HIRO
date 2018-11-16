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
   void setSolution(std:vector<double> x);

   // sets the upper bound
   void setUpperBound(double ub);

   // sets the number of nodes processed
   void setNodes(int nodes);

   // gets the values of the solution x
   std::vector<double> getSolution();

   // gets the upper bound
   double setUpperBound();

   // gets the number of nodes processed
   int setNodes();

   private:
      std::vector<double> _x;
      double _ub;
      int _nodes;
};

#endif
