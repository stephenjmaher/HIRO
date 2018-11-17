SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1271/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1271/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0 -std=c++11

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXJARDIR   = $(CPLEXDIR)/lib/cplex.jar
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread

all: selection

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = $(CPLEXDIR)/examples
EXINC         = $(EXDIR)/include
EXDATA        = $(EXDIR)/data
EXSRCC        = $(EXDIR)/src/c
EXSRCCX       = $(EXDIR)/src/c_x
EXSRCCPP      = $(EXDIR)/src/cpp
EXSRCJAVA     = $(EXDIR)/src/java

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

USRFLAGS =


#------------------------------------------------------------
#  make all      : to compile the examples. 
#  make execute  : to compile and execute the examples.
#------------------------------------------------------------


# ------------------------------------------------------------

clean :
	/bin/rm -rf obj/*.o obj-eval/*.o *~ bin/*

# ------------------------------------------------------------
#
# The examples
#
selection: selection.o
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) -o bin/selection obj/main.o obj/hiro.o obj/hirosolution.o obj/selection.o $(CCLNFLAGS)
selection.o: src/main.cpp src/hiro.cpp src/hirosolution.cpp src/selection.cpp
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src/main.cpp -o obj/main.o
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src/hiro.cpp -o obj/hiro.o
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src/hirosolution.cpp -o obj/hirosolution.o
	$(CCC) -c $(CCFLAGS) ${USRFLAGS} src/selection.cpp -o obj/selection.o

eval: evaluation.o
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) -o bin/evaluation obj-eval/main.o $(CCLNFLAGS)
evaluation.o: src-eval/main.cpp
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src-eval/main.cpp -o obj-eval/main.o

# Local Variables:
# mode: makefile
# End:
