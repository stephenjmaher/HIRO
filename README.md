# HIRO - Hard Instances for Robust Optimization
Methods for generating hard instances for robust optimization problems

This is a start of the HIRO package that will be extended to include many different underlying integer programming problems. The structure of this package is as follows:

- `HIRO` base class containing methods for generating hard instances for robust optimisation
- `HIROsolution` base class to facilitate the transfer of solutions from derived classes to the base class functions
- `Selection` derived class for the selection problem. The pure virtual functions of `solve_ip` and `solve_regret` have been implemented in this derived class

Currently, the `main.cpp` only looks at the `Selection` problem. This needs to be extended. My thought is to create an "examples" folder that will contain each of the integer programs that we want to consider. This is yet to be completed.

# Extending the package

The easiest way to extend the package is to write new derived classes of the `HIRO` class. The `Selection` class is a good example. After writing the new derived class, to use the functions you will need to do the following:

1. modify `main.cpp` to include the appropriate header files.
1. construct an object in `main.cpp` and then call the appropriate functions to generate the hard instances
1. Modify the `Makefile` to compile the new derived class.

# Installation issues

To build the **HIRO** package you will need:

- CPLEX 
  - The path to CPLEX will need to be updated in the Makefile for your distribution. *Please don't commit such changes to the repo*.

Also, the Makefile doesn't create the `obj` and `bin` directories. So for the first installation, you will need to create these directories.

# NOTES

- The `generate_hard_LDR` and `generate_hard_midpoint` functions in the `HIRO` base class have been disabled. This is because the formulations are problem specific. I suspect that these should be made virtual functions, or modify them so that a virtual function could be called to add the appropriate constraints.
