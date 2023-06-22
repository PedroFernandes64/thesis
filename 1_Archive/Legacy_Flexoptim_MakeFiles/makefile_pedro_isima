# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

# $Id: Makefile.in 1898 2013-04-09 18:06:04Z stefan $


##########################################################################
#    You can modify this example makefile to fit for your own program.   #
#    Usually, you only need to change the CHANGEME entries below.  		 #
##########################################################################

# Compiler options
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes 

# CHANGEME: The system where cplex will run
SYSTEM = x86-64_linux

LIBFORMAT = static_pic

# CHANGEME: Cplex paths ok
CPLEXVERSION = CPLEX_Studio1210
CPLEXDIR = /opt/ibm/ILOG/$(CPLEXVERSION)/cplex
CPLEXINCDIR = $(CPLEXDIR)/include/
CPLEXLIBDIR = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CPLEXLIBFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex 

# CHANGEME: Concert paths ok
CONCERTVERSION = concert
CONCERTDIR = /opt/ibm/ILOG/$(CPLEXVERSION)/$(CONCERTVERSION)
CONCERTINCDIR = $(CONCERTDIR)/include/
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBFLAGS = -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -ldl

# CHANGEME: LEMON paths ok
LEMONINCDIR = /home/phfernan/ressources_thesis/lemon/include/
LEMONLIBDIR = /home/phfernan/ressources_thesis/lemon/lib/
LEMONINCFLAGS = -I$(LEMONINCDIR)
LEMONLIBFLAGS = -L$(LEMONLIBDIR) -lemon

# CHANGEME: Boost paths ok
BOOSTINCDIR = /home/phfernan/ressources_thesis/boost_1_78_0/
BOOSTINCFLAGS = -I$(BOOSTINCDIR)

# CHANGEME: CBC flags  OK
OSIFLAGS = /home/phfernan/ressources_thesis/CBCmaster/Osi/


# To compile other examples, either changed the following line, or
# add the argument DRIVER=problem_name to make
DRIVER = exec

# CHANGEME: This should be the name of your executable
EXE = $(DRIVER)

# CHANGEME: Here is the name of all object files corresponding to the source
#           code that you wrote in order to define the problem statement
OBJS = main.o formulation/edgeNodeForm.o formulation/flowForm.o formulation/formulationComponents.o formulation/rsa.o solver/abstractSolver.o solver/solverCplex.o solver/callbackCplex.o solver/solverCBC.o tools/CSVReader.o topology/demand.o topology/input.o topology/instance.o topology/physicalLink.o topology/slice.o lagrangian/formulation/AbstractLagrangianFormulation.o lagrangian/formulation/lagFlow.o lagrangian/formulation/lagNewNonOverlapping.o lagrangian/formulation/lagNonOverlapping.o lagrangian/solver/AbstractLagrangianSolver.o lagrangian/solver/lagSubgradient.o lagrangian/solver/lagVolume.o lagrangian/heuristic/AbstractHeuristic.o lagrangian/heuristic/shortestPathHeuristic.o lagrangian/tools/lagTools.o generator.o solver/lagSolverCBC.o solver/OsiLagSolverInterface.o 

# extra object files that are required by some examples (we just link them into each one)
EXTRAOBJS = \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcCompareUser.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcSolver2.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcSolver3.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcSolverLongThin.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/ClpQuadInterface.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcBranchFollow2.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcBranchUser.o \
	/home/phfernan/ressources_thesis/CBCmaster/Cbc/examples/CbcBranchLink.o 

# CHANGEME: Additional libraries
# e.g., for driver2, put -lOsiCbc here
ADDLIBS = $(CPLEXLIBFLAGS) $(CONCERTLIBFLAGS) $(LEMONLIBFLAGS)

# CHANGEME: Additional flags for compilation (e.g., include flags)
ADDINCFLAGS = -std=c++11 -g $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) $(LEMONINCFLAGS) $(BOOSTINCFLAGS) 

# CHANGEME: Directory to the sources for the (example) problem definition
# files
SRCDIR = .

##########################################################################
#  Usually, you don't have to change anything below.  Note that if you   #
#  change certain compiler options, you might have to recompile the      #
#  package.                                                              #
##########################################################################

COIN_HAS_PKGCONFIG = #TRUE
COIN_CXX_IS_CL = #TRUE
COIN_HAS_SAMPLE = TRUE
COIN_HAS_MIPLIB3 = TRUE

# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS = -O3 -pipe -DNDEBUG -Wparentheses -Wreturn-type -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -Wno-long-long   -DCBC_BUILD

# C Compiler options
CFLAGS = -O3 -pipe -DNDEBUG -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wcast-qual -Wall -Wno-unknown-pragmas -Wno-long-long   -DCBC_BUILD

# additional C++ Compiler options for linking
CXXLINKFLAGS =  -Wl,--rpath -Wl,/usr/local/lib

# Sample data directory
ifeq ($(COIN_HAS_SAMPLE), TRUE)
  ifeq ($(COIN_HAS_PKGCONFIG), TRUE)
    CXXFLAGS += -DSAMPLEDIR=\"`PKG_CONFIG_PATH=/home/phfernan/ressources_thesis/CBCmaster/Cbc/lib/pkgconfig:/home/phfernan/ressources_thesis/CBCmaster/Cbc/libpkgconfig:/home/phfernan/ressources_thesis/CBCmaster/share/pkgconfig:  --variable=datadir coindatasample`\"
      CFLAGS += -DSAMPLEDIR=\"`PKG_CONFIG_PATH=/home/phfernan/ressources_thesis/CBCmaster/Cbc/lib/pkgconfig:/home/phfernan/ressources_thesis/CBCmaster/Cbc/libpkgconfig:/home/phfernan/ressources_thesis/CBCmaster/share/pkgconfig:  --variable=datadir coindatasample`\"
  else
    CXXFLAGS += -DSAMPLEDIR=\"/home/phfernan/ressources_thesis/CBCmaster/share/coin/Data/Sample\"
      CFLAGS += -DSAMPLEDIR=\"/home/phfernan/ressources_thesis/CBCmaster/share/coin/Data/Sample\"
  endif
endif

# Miplib3 data directory
ifeq ($(COIN_HAS_MIPLIB3), TRUE)
  ifeq ($(COIN_HAS_PKGCONFIG), TRUE)
    CXXFLAGS += -DMIPLIB3DIR=\"`PKG_CONFIG_PATH=/home/phfernan/ressources_thesis/CBCmaster/Cbc/lib/pkgconfig:/home/phfernan/ressources_thesis/CBCmaster/Cbc/libpkgconfig:/home/phfernan/ressources_thesis/CBCmaster/share/pkgconfig:  --variable=datadir coindatamiplib3`\"
  else
    CXXFLAGS += -DMIPLIB3DIR=\"/home/phfernan/ressources_thesis/CBCmaster/share/coin/Data/miplib3\"
  endif
endif

# Include directories (we use the CYGPATH_W variables to allow compilation with Windows compilers)
ifeq ($(COIN_HAS_PKGCONFIG), TRUE)
  INCL = `PKG_CONFIG_PATH=/home/phfernan/ressources_thesis/CBCmaster/Cbc/lib/pkgconfig:/home/phfernan/ressources_thesis/CBCmaster/Cbc/libpkgconfig:/home/phfernan/ressources_thesis/CBCmaster/share/pkgconfig:  --cflags cbc`
else
  INCL =  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin   -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  -I/home/phfernan/ressources_thesis/CBCmaster/include/coin  
endif
INCL += -I. $(ADDINCFLAGS)

# Linker flags
ifeq ($(COIN_HAS_PKGCONFIG), TRUE)
  LIBS = `PKG_CONFIG_PATH=/home/phfernan/ressources_thesis/CBCmaster/Cbc/lib/pkgconfig:/home/phfernan/ressources_thesis/CBCmaster/Cbc/libpkgconfig:/home/phfernan/ressources_thesis/CBCmaster/share/pkgconfig:  --libs cbc`
else
  ifeq ($(COIN_CXX_IS_CL), TRUE)
    LIBS = -link -libpath:`$(CYGPATH_W) /home/phfernan/ressources_thesis/CBCmaster/lib` libCbcSolver.lib libCbc.lib libOsiCbc.lib  -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsiClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lClpSolver -lClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsi   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCoinUtils -lm    -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCgl   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsiClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lClpSolver -lClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsi   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCoinUtils -lm   
  else
    LIBS = -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCbcSolver -lCbc -lOsiCbc  -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsiClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lClpSolver -lClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsi   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCoinUtils -lm    -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCgl   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsiClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lClpSolver -lClp   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lOsi   -L/home/phfernan/ressources_thesis/CBCmaster/lib -lCoinUtils -lm   
  endif
endif

# The following is necessary under cygwin, if native compilers are used
CYGPATH_W = echo

# Here we list all possible generated objects or executables to delete them
CLEANFILES = $(EXTRAOBJS) \
	allCuts.o allCuts \
	barrier.o barrier \
	cbc_driverC_sos.o cbc_driverC_sos \
	crew.o crew \
	driver.o driver \
	driver2.o driver2 \
	driver3.o driver3 \
	driver4.o driver4 \
	fast0507b.o fast0507b \
	fast0507.o fast0507 \
	gear.o gear \
	hotstart.o hotstart \
	interrupt.o interrupt \
	link.o link \
	longthin.o longthin \
	lotsize.o lotsize \
	minimum.o minimum \
	modify.o modify \
	nway.o nway \
	qmip.o qmip \
	qmip2.o qmip2 \
	repeat.o repeat \
	sample1.o sample1 \
	sample2.o sample2 \
	sample3.o sample3 \
	sample4.o sample4 \
	sample5.o sample5 \
	simpleBAB.o simpleBAB \
	sos.o sos \
	sudoku.o sudoku

all: $(EXE)

.SUFFIXES: .cpp .c .o .obj

$(EXE): $(OBJS) $(EXTRAOBJS)
	bla=;\
	for file in $(OBJS) $(EXTRAOBJS); do bla="$$bla `$(CYGPATH_W) $$file`"; done; \
	$(CXX) $(CXXLINKFLAGS) $(CXXFLAGS) -o $@ $$bla $(LIBS) $(ADDLIBS)

clean:
	rm -rf $(CLEANFILES) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ `test -f '$<' || echo '$(SRCDIR)/'`$<


.cpp.obj:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(SRCDIR)/$<'; fi`

.c.o:
	$(CC) $(CFLAGS) $(INCL) -c -o $@ `test -f '$<' || echo '$(SRCDIR)/'`$<


.c.obj:
	$(CC) $(CFLAGS) $(INCL) -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(SRCDIR)/$<'; fi`
