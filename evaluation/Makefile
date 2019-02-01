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
# Evaluation details
# ---------------------------------------------------------------------

VERBOSE = false

SRCDIR = src
OBJDIR = obj
BINDIR = bin

MAINNAME 		= 	evaluation
MAINOBJ 			= 	main.o
MAINSRC			= 	$(addprefix $(SRCDIR)/,$(MAINOBJ:.o=.cpp))
MAINOBJFILES	=	$(addprefix $(OBJDIR)/,$(MAINOBJ))
MAINFILE			= $(BINDIR)/$(MAINNAME)

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0 -std=c++11
AR = ar rcs

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

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -Wno-ignored-attributes

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
	$(CCC) $(CCFLAGS) $(USRFLAGS) $(CCLNDIRS) $(MAINOBJFILES) $(CCLNFLAGS) -o $(MAINFILE)

.PHONY: clean
clean :
	@echo "removing obj, bin and lib files"
	/bin/rm -rf obj/*.o obj-eval/*.o *~ bin/* lib/*

$(MAINFILE): $(LIBDIR) $(OBJDIR) $(MAINOBJFILES)

$(OBJDIR):
	@echo "creating $@ directory"
	@-mkdir -p $(OBJDIR)

$(LIBDIR):
	@echo "creating $@ directory"
	@-mkdir -p $(LIBDIR)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	@echo "-> compiling $@"
	$(CCC) -c $(CCFLAGS) $(USRFLAGS) $< -o $@

# Local Variables:
# mode: makefile
# End:
