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
# HIRO details
# ---------------------------------------------------------------------

VERBOSE = false

SRCDIR = src
OBJDIR = obj
LIBDIR = lib

HIRONAME 		= 	hiro
HIROOBJ 			= 	hiro.o \
			  			hirosolution.o
HIROSRC			= 	$(addprefix $(SRCDIR)/,$(HIROOBJ:.o=.cpp))
HIROOBJFILES	=	$(addprefix $(OBJDIR)/,$(HIROOBJ))
HIROLIBFILE		= $(LIBDIR)/lib$(HIRONAME).a

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

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -Wno-ignored-attributes

USRFLAGS =

# ------------------------------------------------------------
# Build commands
# ------------------------------------------------------------

ifeq ($(VERBOSE),false)
.SILENT:	$(MAINFILE) $(MAINOBJFILES) $(MAINSHORTLINK)
endif

.PHONY: all
all: $(HIROLIBFILE)
	@echo "-> linking $<"
	$(AR) $(HIROLIBFILE) $(HIROOBJFILES)

.PHONY: clean
clean :
	@echo "removing obj, bin and lib files"
	/bin/rm -rf obj/*.o obj-eval/*.o *~ bin/* lib/*

$(HIROLIBFILE): $(LIBDIR) $(OBJDIR) $(HIROOBJFILES)

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
