#ifndef __input__h
#define __input__h

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string>
#include <vector>
#include <climits>
#include <unordered_map>
#include <boost/algorithm/string.hpp>

/*****************************************************************************************
 * This class contains all the information needed for the creation of an Instance and the 
 * proper resolution of the RSA problem. It stores input/output file paths, execution and 
 * control parameters.						
*****************************************************************************************/
class Input {

public: 
	/** Enumerates the possible MIP formulations for modelling the RSA problem.**/
	enum Formulation {
		FORMULATION_FLOW = 0,  		/**< The RSA problem is solved using the Flow based formulation. **/
		FORMULATION_EDGE_NODE = 1,	/**< The RSA problem is solved using the Edge-Node formulation. **/
		FORMULATION_T_FLOW = 2,		/**< The RSA problem is solved using the T-Flow formulation. **/
		FORMULATION_DRAOV = 3 		/**< The RSA problem is solved using the DR-AOV formulation. **/
	};

	/** Enumerates the possible solvers to be used for solving the RSA formulation.**/
	enum MIP_Solver {						
		MIP_SOLVER_CPLEX = 0,  	/**< The MIP is solved using CPLEX. **/
		MIP_SOLVER_CBC = 1, 	/**< The MIP is solved using CBC. **/
		MIP_SOLVER_GUROBI = 2,  /**< The MIP is solved using Gurobi. #TODO Implement gurobi.**/
		MIP_SOLVER_SCIP = 3		/**< The MIP is solved using SCIP. **/
	};


	/** Enumerates the possible methods to be applied at each node of the enumeration tree (from Branch-and-Bound or Branch-and-Cut).**/
	enum NodeMethod {
		NODE_METHOD_LINEAR_RELAX = 0,  		/**< At each node of the enumeration tree, Linear Relaxation is applied. **/
		NODE_METHOD_SUBGRADIENT = 1, 		/**< At each node of the enumeration tree, the Subgradient algorithm is applied. #TODO Implement subgradient inside nodes. **/
		NODE_METHOD_VOLUME = 2 				/**< At each node of the enumeration tree, the Volume algorithm is applied. #TODO Implement volume. **/
	};

	/** Enumerates the possible methods to be applied at the root **/
	enum RootMethod{
		ROOT_METHOD_AUTO = 0,
		ROOT_METHOD_PRIMAL = 1,
		ROOT_METHOD_DUAL = 2,
		ROOT_METHOD_NETWORK = 3,
		ROOT_METHOD_BARRIER = 4
	};

	/** Enumerates the possible objectives to be optimized. **/
	enum ObjectiveMetric {
		OBJECTIVE_METRIC_0 = 0,		/**< Minimize nothing, just search for a feasible solution. **/
		OBJECTIVE_METRIC_1 = 1,		/**< Minimize the sum of (max used slice positions) over demands. **/
		OBJECTIVE_METRIC_SLUS = 11,	/**< Minimize the sum of (max used slice positions) over edges. **/
		OBJECTIVE_METRIC_SULD = 2,		/**< Minimize the sum of (number of hops in paths) over demands. **/
		OBJECTIVE_METRIC_TUS = 3,	/**< Minimize the sum of occupied slices. **/
		OBJECTIVE_METRIC_TRL = 4,		/**< Minimize the path lengths. **/
		OBJECTIVE_METRIC_TASE = 44,	/**< Minimize the amplifiers. **/
		OBJECTIVE_METRIC_NLUS = 8,		/**< Minimize the max used slice position overall. **/
		OBJECTIVE_METRIC_TOS = 10, 	/**< Minimize the sum of differences between the OSNR of a path ant the OSNR limit **/
		OBJECTIVE_METRIC_ADS = 1010, 	/**< Minimize the weighted sum of rejected demands **/
		OBJECTIVE_METRIC_LLB = 20, 	/**< Minimize the number of links with installed L band **/
		OBJECTIVE_METRIC_DCB = 2020 	/**< naximize the number of demands in the c band **/
	};

	/** Enumerates the possible output policies to be used. **/
	enum OutputLevel {
		OUTPUT_LVL_NO = 0,			/**< Do not create any output file. **/
		OUTPUT_LVL_NORMAL = 1,		/**< Generate output files corresponding to the last mapping. **/
		OUTPUT_LVL_DETAILED = 2		/**< Generate output files after every optimization procedure. **/
	};

	/** Enumerates the possible spectrum partitioning policies to be applied. **/
	enum PartitionPolicy {
		PARTITION_POLICY_NO = 0,	/**< No spectrum partition. **/
		PARTITION_POLICY_SOFT = 1,	/**< A subset of demands is pushed to the Right region of the spectrum while the rest of it is pushed to the Left region. **/
		PARTITION_POLICY_HARD = 2	/**< The spectrum is divided into two fixed regions (Left and Right). **/
	};
	
	/** Enumerates the possible levels of preprocessing to be applied for eliminating variables before optimization is called. **/
	enum PreprocessingLevel {
		PREPROCESSING_LVL_NO = 0,		/**< Only remove arcs that do not fit the demand load. **/
		PREPROCESSING_LVL_PARTIAL = 1,	/**< Previous levels + look for arcs that would induce length violation and arcs whose neighboors cannot forward the demand. **/
		PREPROCESSING_LVL_FULL = 2		/**< Previous levels recursively until no additional arc can be removed. **/
	};

	/******** INCLUSION FOR LAGRANGIAN *********/

	
	/*******************************************/
	
private:
	const std::string PARAMETER_FILE;				/**< Path to the file containing all the parameters. **/
	std::string topologyFile;						/**< Path to the file containing information on the physical topology of the network.**/
	std::string initialMappingDemandFile;			/**< Path to the file containing information on the already routed demands. **/
	std::string initialMappingAssignmentFile;		/**< Path to the file containing information on the assignment of demands (i.e., on which edge/slice each demand is routed).**/
	std::string demandToBeRoutedFolder;				/**< Path to the folder containing the files on the non-routed demands. **/
	std::vector<std::string> demandToBeRoutedFile;	/**< A vector storing the paths to the files containing information on the non-routed demands. **/
	std::string outputPath;							/**< Path to the folder where the output files will be sent by the end of the optimization procedure.**/

	std::string QoTFolder;						/**< Path to the folder containing auxiliary files for QoT estimation.**/
	
	NodeMethod chosenNodeMethod;			/**< Refers to which method is applied for solving each node.**/
	RootMethod chosenRootMethod;			/**< Refers to which method is applied for solving the root. **/
	Formulation chosenFormulation;			/**< Refers to the formulation used to solve the problem. **/
	MIP_Solver chosenMipSolver;				/**< Refers to the MIP solver chosen to applied. **/
	PreprocessingLevel chosenPreprLvl;		/**< Refers to which level of preprocessing is applied before solving the problem.**/
	std::vector<ObjectiveMetric> chosenObj;	/**< Refers to which objective is optimized.**/
	OutputLevel chosenOutputLvl;			/**< Refers to which output policy is adopted.**/
	PartitionPolicy chosenPartitionPolicy;	/**< Refers to which partition policy is adopted.**/


    int nbDemandsAtOnce;				/**< How many demands are treated in a single optimization.**/
	int nbSlicesInOutputFile;			/**< How many slices will be displayed in the output file. **/
	int partitionSlice;					/**< Refers to the max slice of Left spectrum region, if partitioning policy is set to Hard. **/
	int partitionLoad;					/**< Refers to the max load that can be routed on the Left spectrum region, if some partioning policy is set. **/
	int timeLimit;						/**< Refers to how much time (in seconds) can be spent during one optimization. **/
	int globalTimeLimit;				/**< Refers to how much time (in seconds) can be spent during the whole optmization. **/
	bool allowBlocking;					/**< If this option is inactive, optimization stops within first blocking. Otherwise, blocking is accepted (this only works in online case). **/
	int hopPenalty;						/**< Refers to the penalty of reach applied on each hop. **/
	bool linearRelaxation;				/**< If this option is active, all variables are real (i.e., runs linear relaxation). **/
	bool userCuts;						/**< If this option is active, apply user cuts. **/

	bool GNPY_activation;				/**< If this option is active, the solution provided is guaranteed to satisfy GNPY constraints. Whenever a candidate solution is found, GNPY is called to validade or to reject such solution. **/

	/******** INCLUSION FOR THESIS PEDRO *********/

	bool chromaticDispersionActivation;				/**< If Max Reach constraints will be used. **/
	bool osnrActivation;					/**< If OSNR constraints will be used. **/
	int nbBands;							/**< Number of bands. **/
	int allPathsCompute;
	int allPathsPrint;
	int nonOverlappingTypeTFlow;
	int reinforcements;
	int relaxationVariables;
	int geneticUse;
	Input::ObjectiveMetric geneticMetric;
	int geneticIterations;
	int geneticPopulation;
	int geneticCrossing;
	int geneticMutation;
	int geneticChosenK;
	int geneticExtraK;
	bool lbActive;
	int cplexStrategy;

public:
	/****************************************************************************************/
	/*									Constructors										*/
	/****************************************************************************************/
	/** Default constructor initializes the object with the information contained in the parameterFile. @param file The address of the parameter file (usually the address of file 'onlineParameters'). **/
    Input(std::string file);

	/** Copy constructor. @param i The input to be copied. **/
    Input(const Input &i);

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the path to the file containing all the parameters. **/
    const std::string getParameterFile() const { return PARAMETER_FILE; }

	/** Returns the path to the file containing information on the physical topology of the network.**/
    std::string getTopologyFile() const { return topologyFile; }
	
	/** Returns the path to the file containing information on the already routed demands. **/
    std::string getInitialMappingDemandFile() const { return initialMappingDemandFile; }

	/** Returns the path to the file containing information on the assignment of demands (i.e., on which edge/slice each demand is routed).**/
    std::string getInitialMappingAssignmentFile() const { return initialMappingAssignmentFile; }
	
	/** Returns the path to the folder containing the files on the non-routed demands. **/
    std::string getDemandToBeRoutedFolder() const { return demandToBeRoutedFolder; }

	/** Returns the number of online demand files to be treated. **/
	int getNbDemandToBeRoutedFiles(){ return int(demandToBeRoutedFile.size()); }
	
	/** Returns the vector storing the paths to the files containing information on the non-routed demands. **/
    std::vector<std::string> getDemandToBeRoutedFiles() const { return demandToBeRoutedFile; }

	/** Returns the path to the i-th file containing information on the non-routed demands. @param i The index of file. **/
    std::string getDemandToBeRoutedFilesFromIndex(int i) const { return demandToBeRoutedFile[i]; }

	/** Returns the path to the folder where the output files will be sent by the end of the optimization procedure.**/
    std::string getOutputPath() const { return outputPath; }


    std::string getQotFolder() const { return QoTFolder; }
		
	/** Returns the number of demands to be treated in a single optimization. **/
    int getNbDemandsAtOnce() const { return nbDemandsAtOnce; }

	/** Returns the number of slices to be displayed in the output file. **/
    int getnbSlicesInOutputFile() const { return nbSlicesInOutputFile; }

	/** Returns the max slice of Left spectrum region. **/
    int getPartitionSlice() const { return partitionSlice; }

	/** Returns the max load that should be pushed to the Left spectrum region. **/
    int getPartitionLoad() const { return partitionLoad; }

	/** Returns the timeLimit for a single optimization iteration. **/
    int getIterationTimeLimit() const { return timeLimit; }

	/** Returns the global time limit applied to the optimization. **/
    int getOptimizationTimeLimit() const { return globalTimeLimit; }

	/** Returns true if blocking is accepted and false, otherwise. **/
    bool isBlockingAllowed() const { return allowBlocking; }

	/** Returns true if linear relaxation is applied. **/
    bool isRelaxed() const { return linearRelaxation; }

	/** Returns true if user cuts are applied. **/
    bool isUserCutsActivated() const { return userCuts; }

	/** Returns the hop penality. **/
    int getHopPenalty() const { return hopPenalty; }

	/** Returns true if GNPY should be used. **/
    bool isGNPYEnabled() const { return GNPY_activation; }
	
	//PEDRO PEDRO PEDRO
	bool isMaxCDEnabled() const { return chromaticDispersionActivation; }
	bool isMinOSNREnabled() const { return osnrActivation; }
	bool computeAllPaths() const { return allPathsCompute; }
	bool printAllPaths() const { return allPathsPrint; }
	int getNbBands() const { return nbBands; }
	int getNonOverTFlow() const { return nonOverlappingTypeTFlow; }
	bool areReinforcementsEnabled() const { return reinforcements; }
	bool recoverRelaxationVariables() const { return relaxationVariables; }
	bool activateGeneticAlgorithm() const { return geneticUse; }
	Input::ObjectiveMetric geneticAlgorithmMetric() const { return geneticMetric; }
	int geneticAlgorithmIterations() const { return geneticIterations; }
	int geneticAlgorithmPopulation() const { return geneticPopulation; }
	int geneticAlgorithmCrossing() const { return geneticCrossing; }
	int geneticAlgorithmMutation() const { return geneticMutation; }
	int geneticGetChosenK() const { return geneticChosenK; }
	int geneticGetExtraK() const { return geneticExtraK; }
	bool activateLB() const { return lbActive; }
	int getCplexStrategy() const { return cplexStrategy; }
	
	bool isObj8(int i) const;

	/** Returns the identifier of the method chosen for solving each node. **/
    const NodeMethod & getChosenNodeMethod() const { return chosenNodeMethod; }

	/** Returns the identifier of the method chosen for solving the root. **/
    const RootMethod & getChosenRootMethod() const { return chosenRootMethod; }

	/** Returns the identifier of the formulation chosen for solving the problem. **/
    const Formulation & getChosenFormulation() const { return chosenFormulation; }

	/** Returns the identifier of the MIP solver chosen for solving the formulation. **/
    const MIP_Solver & getChosenMIPSolver() const { return chosenMipSolver; }

	/** Returns the identifier of the chosen preprocessing level. **/
    const PreprocessingLevel & getChosenPreprLvl() const { return chosenPreprLvl; }

	/** Returns the identifier of the objective chosen to be optimized. **/
    const ObjectiveMetric & getChosenObj_k(int i) const { return chosenObj[i]; }

	/** Returns the name of the objective. @param obj The chosen objective. **/
	std::string getObjName(ObjectiveMetric obj) const;

	/** Returns the vector of objectives chosen to be optimized. **/
    std::vector<ObjectiveMetric> getChosenObj() const { return chosenObj; }

	/** Returns the identifier of the output policy adopted. **/
    const OutputLevel & getChosenOutputLvl() const { return chosenOutputLvl; }

	/** Returns the identifier of the partition policy adopted. **/
    const PartitionPolicy & getChosenPartitionPolicy() const { return chosenPartitionPolicy; }

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	
	/** Changes the number of demands to be treated in a single optimization. @param val The new number of demands. **/
    void setNbDemandsAtOnce(const int val) { nbDemandsAtOnce = val; }

	/** Changes the time limit of one iteration. @param val The new time limit (in seconds). **/
    void setTimeLimit(const int val) { timeLimit = val; }

	/** Changes the global time limit. @param val The new time limit (in seconds). **/
    void setGlobalTimeLimit(const int val) { globalTimeLimit = val; }

	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	/** Searches for a pattern in the parameter file and returns its associated value. @param pattern The substring pattern to be searched. @note It is surely not the most performant method but the parameter file has a reasonable size and the code becomes much clearer. **/
    std::string getParameterValue(std::string pattern);

	/** Populates the vector of paths to the files containing information on the non-routed demands. **/
	void populateOnlineDemandFiles();

	/** Converts a string into an ObjectiveMetric vector. **/
	std::vector<ObjectiveMetric> to_ObjectiveMetric(std::string data);

	/** Converts a string into a PartitionPolicy. **/
	PartitionPolicy to_PartitionPolicy(std::string data);

	/** Converts a string into a NodeMethod. **/
	NodeMethod to_NodeMethod(std::string data);

	/** Converts a string into a RootMethod. **/
	RootMethod to_RootMethod(std::string data);

	/** Converts a string into a Formulation. **/
	Formulation to_Formulation(std::string data);

	/** Converts a string into a MIP_Solver. **/
	MIP_Solver to_MIP_Solver(std::string data);

	/** Converts a string into time limit. \note By default, time limit is unlimited. **/
	int to_timeLimit(std::string data);

	/** Displays the main input file paths: link, demand and assignement. **/
    void displayMainParameters();
	
	/** Checks if the gathered information is consistent with what is implemented. **/
	void checkConsistency();

	/****************************************************************************************/
	/*										Destructor										*/
	/****************************************************************************************/

	/** Destructor. Clears the vector of strings. **/
	~Input();
};

#endif