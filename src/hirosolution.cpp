#include "hirosolution.h"

using namespace std;

namespace hiro
{
   // default constructor
   HIROsolution::HIROsolution()
      : _x(0)
      , _ub(-HIRO::INFINITY)
      , _nodes(-1)
   {}

   // constructor with inputs
   HIROsolution::HIROsolution(std::vector<double> x, double ub, int nodes)
      : _x(x)
      , _ub(ub)
      , _nodes(nodes)
   {}

   // assignment operator
   //HIROsolution::HIROsolution& operator=(const HIROsolution& rhs)

   // copy constructor
   //HIROsolution::HIROsolution(const HIROsolution& rhs)

   // sets the values of the solution x
   void HIROsolution::setSolution(std:vector<double> x)
   {
      _x = x;
   }

   // sets the upper bound
   void HIROsolution::setUpperBound(double ub)
   {
      _ub = ub;
   }

   // sets the number of nodes processed
   void HIROsolution::setNodes(int nodes)
   {
      assert(nodes >= 0);
      _nodes = nodes;
   }

   // gets the values of the solution x
   std::vector<double> HIROsolution::getSolution()
   {
      return _x;
   }

   // gets the upper bound
   double HIROsolution::setUpperBound()
   {
      return _ub
   }

   // gets the number of nodes processed
   int HIROsolution::setNodes()
   {
      return _nodes;
   }

} // namespace hiro
