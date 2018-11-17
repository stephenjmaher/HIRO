#include "hiro.h"

using namespace std;

namespace hiro
{
   // default constructor
   HIROsolution::HIROsolution()
      : _x(0)
      , _ub(INFTY)
      , _nodes(-1)
   {}

   // constructor with inputs
   HIROsolution::HIROsolution(std::vector<double> x, double ub, int nodes)
      : _x(x)
      , _ub(ub)
      , _nodes(nodes)
   {}

   // default destructor
   HIROsolution::~HIROsolution()
   {
   }

   // assignment operator
   //HIROsolution::HIROsolution& operator=(const HIROsolution& rhs)

   // copy constructor
   //HIROsolution::HIROsolution(const HIROsolution& rhs)

   // sets the values of the solution x
   void HIROsolution::set_solution(std::vector<double> x)
   {
      _x = x;
   }

   // sets the upper bound
   void HIROsolution::set_upper_bound(double ub)
   {
      _ub = ub;
   }

   // sets the number of nodes processed
   void HIROsolution::set_nodes(int nodes)
   {
      assert(nodes >= 0);
      _nodes = nodes;
   }

   // gets the values of the solution x
   std::vector<double> HIROsolution::get_solution()
   {
      return _x;
   }

   // gets the upper bound
   double HIROsolution::get_upper_bound()
   {
      return _ub;
   }

   // gets the number of nodes processed
   int HIROsolution::get_nodes()
   {
      return _nodes;
   }
} // namespace hiro
