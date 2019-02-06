#include "hiro.h"

using namespace std;

namespace hiro
{
   // default constructor
   HIROsolution::HIROsolution()
      : _x(0)
      , _ub(INFTY)
   {}

   // constructor with inputs
   HIROsolution::HIROsolution(std::vector<double> x, double ub)
      : _x(x)
      , _ub(ub)
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
} // namespace hiro
