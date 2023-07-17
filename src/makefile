
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
LEMONINCDIR = /usr/local/include
LEMONLIBDIR = /usr/local/lib
LEMONINCFLAGS = -I$(LEMONINCDIR)
LEMONLIBFLAGS = -L$(LEMONLIBDIR) -lemon

# CHANGEME: Boost paths ok
BOOSTINCDIR = /user/include/boost/
BOOSTINCFLAGS = -I$(BOOSTINCDIR)

# To compile other examples, either changed the following line, or
# add the argument DRIVER=problem_name to make
DRIVER = exec

# CHANGEME: This should be the name of your executable
EXE = $(DRIVER)

# CHANGEME: Here is the name of all object files corresponding to the source
#           code that you wrote in order to define the problem statement
OBJS = main.o formulation/edgeNodeForm.o formulation/flowForm.o formulation/formulationComponents.o formulation/rsa.o solver/abstractSolver.o solver/solverCplex.o solver/callbackCplex.o tools/CSVReader.o topology/demand.o topology/input.o topology/instance.o topology/physicalLink.o topology/slice.o solver/solverSCIP.o

# extra object files that are required by some examples (we just link them into each one)
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

# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS = -O3 -pipe -DNDEBUG -Wparentheses -Wreturn-type -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -Wno-long-long -DCBC_BUILD 

# C Compiler options
CFLAGS = -O3 -pipe -DNDEBUG -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wcast-qual -Wall -Wno-unknown-pragmas -Wno-long-long -DCBC_BUILD

# additional C++ Compiler options for linking
CXXLINKFLAGS =  -Wl,--rpath -Wl,/usr/local/lib

INCL += -I. $(ADDINCFLAGS)

# The following is necessary under cygwin, if native compilers are used
CYGPATH_W = echo

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
 
# CHANGEME: SCIP paths
SCIPLIBDIR = /
SCIPLIBFLAGS = -L$(SCIPLIBDIR) -lscip

# CHANGEME: SCIP include directories
#SCIPIFLDIR = /home/kafavoreto/Projeto/include
#SCIPIBIDIR = /home/kafavoreto/Projeto/include/scip
#SCIPLIBIDIR = /home/kafavoreto/Projeto/include/scip/pub
#SCIPIINCFLAGS = -I$(SCIPIFLDIR) -I$(SCIPIBIDIR) -I$(SCIPLIBIDIR)

# CHANGEME: Additional libraries
# e.g., for driver2, put -lOsiCbc here
ADDLIBS = $(CPLEXLIBFLAGS) $(CONCERTLIBFLAGS) $(SCIPLIBFLAGS) $(LEMONLIBFLAGS)

# CHANGEME: Additional flags for compilation (e.g., include flags)
ADDINCFLAGS = -std=c++11 -g $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) $(SCIPIINCFLAGS) $(LEMONINCFLAGS) $(BOOSTINCFLAGS)
