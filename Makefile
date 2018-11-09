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

all: select

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
select: select.o
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) -o bin/select obj/main.o obj/select.o $(CCLNFLAGS)
select.o: src/main.cpp src/select.cpp
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src/main.cpp -o obj/main.o
	$(CCC) -c $(CCFLAGS) ${USRFLAGS} src/select.cpp -o obj/select.o

eval: evaluation.o
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) -o bin/evaluation obj-eval/main.o $(CCLNFLAGS)
evaluation.o: src-eval/main.cpp
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) src-eval/main.cpp -o obj-eval/main.o

# Local Variables:
# mode: makefile
# End:
