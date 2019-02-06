# HIRO - Hard Instances for Robust Optimization
Methods for generating hard instances for robust optimization problems

This is a start of the HIRO package that will be extended to include many different underlying integer programming problems. The structure of this package is as follows:

- `HIRO` base class containing methods for generating hard instances for robust optimisation
- `HIROsolution` base class to facilitate the transfer of solutions from derived classes to the base class functions

These two classes comprise the **HIRO** library. This library is used in the examples for creating difficult instances for different combinatorial optimisation problems. However, the library is not limited to combinatorial optimisation problems and can be used to generate hard robust optimisation instances for general mixed integer programs.

## Examples
There are currently two examples in the **HIRO** package

### Selection
This consists of the following files:
- `main.cpp` the main file for executing the HIRO package to generate hard instances for the selection problem.
- `selection.cpp` and `selection.h` containing the derived class for the selection problem. The pure virtual function of `solve_ip` has been implemented in this derived class

### Traveling Salesman Problem
This consists of the following files:
- `main.cpp` the main file for executing the HIRO package to generate hard instances for the TSP.
- `tsp.cpp` and `tsp.h` containing the derived class for the TSP. The pure virtual functions of `solve_ip` has been implemented in this derived class

# Extending the package

The easiest way to extend the package is to write a new example. It is possible to copy one of the current examples and then modify it to suit your needs. The basic steps for creating a new example are (we will call this new example *Elephant*):

1. copy the *xyz* example directory and rename it **elephant**. The files and directories needed are the `src` directory, including its contents, and the `Makefile`.
1. change to the `elephant` directory.
1. replace all occurences of `xyz` to `elephant` (preserving case) in the `Makefile` and all of the source files in `src`.
1. in the `src` directory, rename `xyz.cpp` and `xyz.cpp` to `elephant.h` and `elephant.cpp` respectively.
1. implement the `solve_ip()` so that the integer program for *Elephant* can be solved within the **HIRO** package.
1. open `main.cpp`, modify this file to input the necessary parameters for the *Elephant* integer program.

# Installation

You need to first build the **HIRO** library and then build your example. Follow these steps

1. in the `hiro` base directory, open the `Makefile` and modify the path to CPLEX and CONCERT.
1. call `make`
1. change to your example directory, such as `selection`.
  1. modify the path to CPLEX and CONCERT
  1. call `make`

## Installation issues

The Makefile doesn't create the `obj` and `bin` directories. So for the first installation, you will need to create these directories.

# NOTES

- The `generate_hard_LDR` and `generate_hard_midpoint` functions in the `HIRO` base class have been disabled. This is because the formulations are problem specific. I suspect that these should be made virtual functions, or modify them so that a virtual function could be called to add the appropriate constraints.
