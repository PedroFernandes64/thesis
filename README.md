# FlexOptim
This is a repository for the online algorithms used for solving the RSA problem from FlexOptim project.


# Requirements
- This code was developped for linux. If you use Windows, please install Windows Subsystem Linux (WSL) by following the steps described here: https://docs.microsoft.com/en-us/windows/wsl/install-win10

- The COIN-OR lemon library is used to model graphs. Lemon can be installed by followig the instructions described here:
https://lemon.cs.elte.hu/trac/lemon/wiki/InstallGuide

- Boost library is also used for simplifying the lecture from .csv files. Boost library can found here: 
https://www.boost.org/doc/libs/1_55_0/doc/html/bbv2/installation.html

- This code uses IBM ILOG CPLEX version 12.10 (more recent versions are also supported) which can be installed by following the instructions here: 
https://developer.ibm.com/docloud/blog/2019/07/04/cplex-optimization-studio-for-students-and-academics/

- The COIN-OR CBC library is used for solving MIPs in an open surce way. CBC can be installed following the instructions described here:
https://projects.coin-or.org/Cbc

- The OSNR of a route can be precisely computed using GNPY. If GNPY is enabled, it should be installed following the instructions described here:
https://gnpy.readthedocs.io/en/master/install.html

# Compilation
The compilation of the project is done through a makefile. If you have all the requirements described above, just update the CHANGEME entries in the makefile to the correspondent directories where you have installed each of the libraries.

Open a terminal in linux (or WSL), go to the project folder and type make. Some warnings are expected but harmless.

# Running
After compiling the project, one can execute it by typing ./exec onlineParameters

# Parameters
The file onlineParameters contains the parameters needed for the program to run properly.
- topologyFile: Refers to the address of the file containing information on the physical topology links.
- initialMappingDemandFile: Refers to the address of the file containing information on the demands present in the initial mapping.
- initialMappingAssignmentFile: Refers to the address of the file containing information on the routing and spectrum assignment of the demands present in the initial mapping.
- demandToBeRoutedFolder: Refers to the address of the folder containing all files of demands to be routed.
- GNPY_activation: Boolean parameter stating whether GNPY is active.
- GNPY_topologyFile: In order to use GNPY, the topology must be precisely defined. This refers to the address of the file containing the specific topology information.
- GNPY_equipmentFile: In order to use GNPY, the equipments used in the network must be precisely defined. This refers to the address of the file containing the specific equipment information.
- nbDemandsAtOnce: States how many demands are treated in one optimization step.
- formulation: Choice of the formulation to be used. 0 for Flow Formulation. 1 for Edge-Node.
- userCuts: Boolean parameter indicating whether user cuts should be applied.
- obj: Which objective to be optimized. 1 for minimize last slot used per demand. 1p for minimize last slot used per edge. 2 for minimize number of hops. 2p for minimize used slices. 4 minimize path length. 8 minimize last slot used overall.
- allowBlocking: Boolean parameter indicating whether the optimization can continue after a demand is blocked.
- hopPenalty: Demand's reach penalty applied on each hop.
- partitionPolicy: States whether the spectrum is partitioned. 0 for no partition at all. 1 for pushing bigger demands to the right and smaller to the left.
- partitionLoad: States the highest load that should be on the left partition.
- partitionSlice: If partition policy = 2, states where the spectrum is divided.
- solver: Which solver to be used. 0 for CPLEX. 1 for CBC.
- method: Possible methods to be applied at each node of the enumeration tree. 0 for linear relaxation.
- preprocessingLevel: Possible levels of preprocessing to be applied for eliminating variables before optimization is called. 0 to only remove arcs that do not fit the demand load. 1 to look for arcs that would induce length violation and arcs whose neighboors cannot forward the demand. 2 to apply level 1 recursevely until no arc can be removed.
- linearRelaxation: Boolean parameter used to turn integer variables into linear ones. 
- outputPath refers to the folder address where the output files will be sent by the end of the optimization procedure.
- outputLevel: Possible output policies to be used. 0 for not creating any output file. 1 for generating output files corresponding to the last mapping. 2 for generating output files after every optimization iteration.
- nbSlicesInOutputFile: States how many slices will be displayed in the output file
- globalTimeLimit: Refers to how much time (in seconds) can be spent during the whole optmization.
- timeLimit: Refers to how much time (in seconds) can be spent during the one iteration of the optimization.

Next parameters are reserved for team LIMOS.
- lagrangianMultiplier_zero refers to the initial value of the lagrangian multiplier used if subgradient method is chosen.
- lagrangianLambda_zero refers to the initial value of the lambda used for computing the step size if subgradient method is chosen.
- maxNbIterations states the maximal number of iterations subgradient method is allowed.
- nbIterationsWithoutImprovement states the maximal number of iterarions the subgradient method is allowed without improving the lower bound.
