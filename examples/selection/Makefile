SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

# The base directory for the HIRO package needs to be specified
HIRODIR = ../..
HIROLIBDIR = $(HIRODIR)/lib
HIROSRCDIR = $(HIRODIR)/src
HIROLIB = -lhiro

# ---------------------------------------------------------------------
# Example settings
# ---------------------------------------------------------------------

VERBOSE = false

BINDIR = bin
SRCDIR = src
OBJDIR = obj

MAINNAME 		= selection
MAINOBJ 			= main.o selection.o
MAINSRC			=	$(addprefix $(SRCDIR)/,$(MAINOBJ:.o=.cpp))
MAINOBJFILES	=	$(addprefix $(OBJDIR)/,$(MAINOBJ))
MAINFILE			= $(BINDIR)/$(MAINNAME)

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

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(HIROSRCDIR) -Wno-ignored-attributes

USRFLAGS =

# ------------------------------------------------------------
# Build commands
# ------------------------------------------------------------

ifeq ($(VERBOSE),false)
.SILENT:
endif

.PHONY: all
all: $(MAINFILE)
	@echo "-> linking $<"
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) $(MAINOBJFILES) -L$(HIROLIBDIR) $(HIROLIB) $(CCLNFLAGS) -o $(MAINFILE)

.PHONY: clean
clean :
	@echo "removing obj, bin and lib files"
	/bin/rm -rf obj/*.o obj-eval/*.o *~ bin/*

$(MAINFILE): $(BINDIR) $(OBJDIR) $(MAINOBJFILES)

$(OBJDIR):
	@echo "creating $@ directory"
	@-mkdir -p $(OBJDIR)

$(BINDIR):
	@echo "creating $@ directory"
	@-mkdir -p $(BINDIR)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	@echo "-> compiling $@"
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) $< -o $@

# Local Variables:
# mode: makefile
# End:
